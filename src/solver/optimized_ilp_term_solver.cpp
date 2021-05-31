#include "optimized_ilp_term_solver.h"
#include "semantics_factory.h"

#include <set>
#include "bitset.h"
#include "glog/logging.h"
#include "recorder.h"
#include "util.h"
#include <random>

namespace {
    std::mt19937 rng;
    std::uniform_real_distribution<double> distribution(0, 1);

    int program_size_limit;

    Program* buildAssignment(const std::vector<int>& assignment) {
        std::vector<Program*> term_list;
        auto* times = semantics::getSemanticsFromName("*");
        auto* plus = semantics::getSemanticsFromName("+");
        int n = assignment.size() - 1;
        for (int i = 0; i < n; ++i) {
            if (assignment[i]) {
                auto* var = new Program({}, new ParamSemantics(i, TINT));
                if (assignment[i] != 1) {
                    auto *k = new Program({}, new ConstSemantics(assignment[i]));
                    term_list.push_back(new Program({var, k}, times));
                } else {
                    term_list.push_back(var);
                }
            }
        }
        if (assignment[n] || term_list.empty()) {
            term_list.push_back(new Program({}, new ConstSemantics(assignment[n])));
        }
        auto* result = term_list[0];
        for (int i = 1; i < term_list.size(); ++i) {
            result = new Program({result, term_list[i]}, plus);
        }
        return result;
    }

    bool is_print = false;

    bool verifyAssignment(const std::vector<int>& assignment, PointExample* example) {
        int oup = assignment[example->first.size()];
        for (int i = 0; i < example->first.size(); ++i) {
            oup += assignment[i] * example->first[i].getInt();
        }
        return oup == example->second.getInt();
    }

    struct AssignmentInfo {
        std::vector<int> assignment;
        Bitset P;
        int cost;
        AssignmentInfo(const std::vector<int>& _assignment): assignment(_assignment), cost(0) {
            for (int& v: assignment) cost += !!v;
        }
        void update(const std::vector<PointExample*>& example_list) {
            for (int i = P.size(); i < example_list.size(); ++i) {
                P.append(verifyAssignment(assignment, example_list[i]));
            }
        }
    };

    struct AssignmentCmp {
        Bitset all;
        AssignmentCmp(const Bitset& _all): all(_all) {}
        int operator () (AssignmentInfo* info_1, AssignmentInfo* info_2) {
            return (info_1->P & all).count() > (info_2->P & all).count();
        }
    };

    std::map<std::vector<int>, AssignmentInfo*> info_map, solved_sample;

    AssignmentInfo* buildAssignmentInfo(const std::vector<int>& assignment, const std::vector<PointExample*>& example_list) {
        if (info_map.count(assignment)) {
            auto* result = info_map[assignment];
            result->update(example_list);
            return result;
        }
        auto* info = new AssignmentInfo(assignment);
        info->update(example_list);
        info_map[assignment] = info;
        return info;
    }

    struct SampleInfo {
        int status;
        std::vector<int> example_list;
        AssignmentInfo* result;
        SampleInfo(const std::vector<int>& _example_list): example_list(_example_list), status(0), result(nullptr) {
            std::sort(example_list.begin(), example_list.end());
        }
    };

    int K_limit;

    struct TermPlan {
        int N;
        std::vector<AssignmentInfo*> term_list;
        std::vector<int> rem_example;
        Bitset info;
        std::vector<SampleInfo*> sample_list;
        TermPlan(int _N, const std::vector<AssignmentInfo*>& _term_list, const Bitset& _info, const std::vector<SampleInfo*>& _sample_list):
        N(_N), term_list(_term_list), info(_info), sample_list(_sample_list) {}
        TermPlan(int _N, const std::vector<AssignmentInfo*>& _term_list): N(_N), term_list(_term_list) {}

        bool checkCover(SampleInfo* sample) {
            for (int& id: sample->example_list) if (!info[id]) return false;
            return true;
        }

        void print() {
            std::cout << "term plan with n = " << N << " rem_num = " << rem_example.size() << std::endl;
            for (auto* info: term_list) {
                std::cout << buildAssignment(info->assignment)->toString() << std::endl;
            }
        }

        SampleInfo* generateSample(int type, int mid) {
            std::vector<int> id_list(N);
            if (type == 0) {
                for (int i = 0; i < N; ++i) id_list[i] = rem_example[std::rand() % mid];
            } else {
                while (1) {
                    bool is_valid = false;
                    for (int i = 0; i < N; ++i) {
                        int now = rem_example[std::rand() % rem_example.size()];
                        if (now >= mid) is_valid = true;
                        id_list[i] = now;
                    }
                    if (is_valid) break;
                }
            }
            return new SampleInfo(id_list);
        }
    };

    struct TermPlanCmp {
        bool operator () (TermPlan* plan_1, TermPlan* plan_2) {
            if (plan_1->N < plan_2->N) return true;
            if (plan_1->N > plan_2->N) return false;
            if (plan_1->term_list.size() < plan_2->term_list.size()) return true;
            if (plan_1->term_list.size() > plan_2->term_list.size()) return false;
            for (int i = 0; i < plan_1->term_list.size(); ++i) {
                if (plan_1->term_list[i]->assignment == plan_2->term_list[i]->assignment) continue;
                return plan_1->term_list[i]->assignment < plan_2->term_list[i]->assignment;
            }
            return false;
        }
    };

    std::set<TermPlan*, TermPlanCmp> plan_set;

    std::vector<PointExample*> idToExample(const std::vector<int>& id_list, const std::vector<PointExample*>& example_space) {
        std::vector<PointExample*> result;
        for (auto& id: id_list) result.push_back(example_space[id]);
        return result;
    }


    void performSample(GRBEnv& env, SampleInfo* sample, const std::vector<PointExample*>& example_space) {
        if (sample->status) return;
        if (solved_sample.count(sample->example_list)) {
            sample->result = solved_sample[sample->example_list];
            if (sample->result) sample->result->update(example_space);
            return;
        }
        sample->status = 1;
        auto example_list = idToExample(sample->example_list, example_space);
        std::vector<int> assignment;
        recorder::recorder.pushStage("sample");
        int status = -1;
        status = solver::getOptimalAssignment(env, example_list, assignment);
        recorder::recorder.popStage();
        global::sample_num += 1;
        if (status == 0) {
            global::sample_status["FAIL"] += 1;
            solved_sample[sample->example_list] = nullptr;
            return;
        }
        global::sample_status[buildAssignment(assignment)->toString()] += 1;
        sample->result = buildAssignmentInfo(assignment, example_space);
        solved_sample[sample->example_list] = sample->result;
    }

    std::vector<AssignmentInfo*> insertTermList(TermPlan* plan, AssignmentInfo* info) {
        auto result = plan->term_list;
        int pos = 0;
        while (pos < plan->term_list.size() && plan->term_list[pos]->assignment > info->assignment) {
            ++pos;
        }
        result.push_back(nullptr);
        for (int i = result.size() - 1; i > pos; --i) result[i] = result[i - 1];
        result[pos] = info;
        return result;
    }

    int calculateRandomTime(int branch_num, int N) {
        int ti = config::KRandomC;
        for (int i = 1; i <= N; ++i) ti *= branch_num;
        return ti;
    }

    void extendStart(TermPlan* plan, const std::vector<PointExample*>& example_space) {
        int pre_size = plan->info.size();
        int n = plan->N;
        for (int i = pre_size; i < example_space.size(); ++i) {
            plan->info.append(true);
            plan->rem_example.push_back(i);
        }
        for (auto* sample: plan->sample_list) {
            if (sample->result) sample->result->update(example_space);
        }
        std::vector<SampleInfo*> new_sample;
        int max_rem = K_limit - plan->term_list.size();
        int max_sample_num = calculateRandomTime(max_rem, n);
        double p = std::pow(1.0 * pre_size / plan->rem_example.size(), n);
        int pos = 0;
        while (pos < plan->sample_list.size() && new_sample.size() < max_sample_num) {
            if (distribution(rng) <= p) {
                new_sample.push_back(plan->sample_list[pos++]);
            } else {
                new_sample.push_back(plan->generateSample(1, pre_size));
            }
        }
        plan->sample_list = new_sample;
    }

    void extendPlan(TermPlan* plan, AssignmentInfo* info, TermPlan* pre, const std::vector<PointExample*>& example_space) {
        int pre_size = plan->info.size();
        int pre_num = plan->rem_example.size();
        for (int i = pre_size; i < example_space.size(); ++i) {
            if (!info->P[i] && pre->info[i]) {
                plan->info.append(true);
                plan->rem_example.push_back(i);
            } else {
                plan->info.append(false);
            }
        }
        for (auto* sample: plan->sample_list) {
            if (sample->result) sample->result->update(example_space);
        }
        std::vector<SampleInfo*> reusable_result;
        std::vector<SampleInfo*> new_sample;
        int n = plan->N;
        for (auto* sample: pre->sample_list) {
            if (plan->checkCover(sample) && sample->example_list[n - 1] >= pre_size) {
                reusable_result.push_back(sample);
            }
        }
        double p = std::pow(1.0 * pre_num / plan->rem_example.size(), n);
        int l_pos = 0, r_pos = 0;
        int max_rem = K_limit - plan->term_list.size();
        int max_sample_num = calculateRandomTime(max_rem, n);
        while ((l_pos < plan->sample_list.size() || r_pos < reusable_result.size()) && new_sample.size() < max_sample_num) {
            if (distribution(rng) <= p) {
                if (l_pos < plan->sample_list.size()) {
                    new_sample.push_back(plan->sample_list[l_pos++]);
                } else {
                    new_sample.push_back(plan->generateSample(0, pre_num));
                }
            } else {
                if (r_pos < reusable_result.size()) {
                    new_sample.push_back(reusable_result[r_pos++]);
                } else {
                    new_sample.push_back(plan->generateSample(1, pre_num));
                }
            }
        }
        plan->sample_list = new_sample;
    }

    TermPlan* buildTermPlan(TermPlan* plan, AssignmentInfo* info, const std::vector<PointExample*>& example_space) {
        auto term_list = insertTermList(plan, info);
        auto* dummy_plan = new TermPlan(plan->N, term_list);
        auto it = plan_set.find(dummy_plan);
        if (it != plan_set.end()) {
            auto* result = *it; delete dummy_plan;
            extendPlan(result, info, plan, example_space);
            return result;
        } else {
            auto* result = dummy_plan;
            plan_set.insert(result);
            extendPlan(result, info, plan, example_space);
            return result;
        }
    }

    std::vector<AssignmentInfo*> getNextAssignment(GRBEnv& env, TermPlan* plan, const std::vector<PointExample*>& example_space, int N, int rem_branch) {
        int ti = calculateRandomTime(rem_branch, N);
        int limit = (int(plan->rem_example.size()) - 1) / rem_branch + 1;
        while (plan->sample_list.size() < ti) {
            plan->sample_list.push_back(plan->generateSample(0, plan->rem_example.size()));
        }
        std::set<AssignmentInfo*> info_set;
        std::set<AssignmentInfo*> fail_set;
        std::vector<AssignmentInfo*> result;
        for (int i = 0; i < ti; ++i) {
            auto* sample = plan->sample_list[i];
            performSample(env, sample, example_space);
            auto* info = sample->result;
            if (!info) continue;
            if (info_set.find(info) == info_set.end() && (plan->info & info->P).count() >= limit && info->cost <= program_size_limit) {
                info_set.insert(info);
                result.push_back(info);
            } else {
                fail_set.insert(info);
            }
        }
        if (is_print) {
            std::cout << "success" << std::endl;
            for (auto res: info_set) {
                std::cout << buildAssignment(res->assignment)->toString() << " ";
            }
            std::cout << std::endl;
            if (info_set.empty()) {
                std::cout << "fail" << std::endl;
                for (auto res: fail_set) {
                    std::cout << buildAssignment(res->assignment)->toString() << " ";
                }
                std::cout << std::endl;
            }
        }
        return result;
    }

    std::set<TermPlan*> visited_plan;

    bool search(GRBEnv& env, TermPlan* plan, const std::vector<PointExample*>& example_space, std::vector<Program*>& result, int N, int rem_branch) {
        if (rem_branch == 0 || plan->rem_example.empty()) {
            assert(plan->rem_example.empty());
            result.clear();
            for (auto* assignment: plan->term_list) {
                result.push_back(buildAssignment(assignment->assignment));
            }
            return true;
        }
        if (is_print) plan->print();
        auto next_assignment = getNextAssignment(env, plan, example_space, N, rem_branch);
        std::sort(next_assignment.begin(), next_assignment.end(), AssignmentCmp(plan->info));
        for (auto* info: next_assignment) {
            if (util::checkTimeOut()) continue;
            auto* next_plan = buildTermPlan(plan, info, example_space);
            if (visited_plan.find(next_plan) == visited_plan.end()) {
                visited_plan.insert(next_plan);
                if (search(env, next_plan, example_space, result, N, rem_branch - 1)) return true;
            }
        }
        return false;
    }

    void releaseBound() {
        config::KTermIntMax *= 10;
        config::KMaxBranchNum += 1;
        for (auto* plan: plan_set) {
            for (auto* sample: plan->sample_list) {
                if (sample->status && !sample->result) {
                    sample->status = 0;
                }
            }
        }
        for (auto& sample_pair: solved_sample) {
            if (!sample_pair.second) solved_sample.erase(sample_pair.first);
        }
    }
}

std::vector<Program *> OptimizedTermSolver:: getTerms(const std::vector<PointExample *> &example_space, int N, int K) {
    visited_plan.clear();
    std::vector<int> full_example;

    auto* start = new TermPlan(N, {});
    auto it = plan_set.find(start);
    if (it == plan_set.end()) {
        plan_set.insert(start);
    } else {
        delete start; start = *it;
    }
    extendStart(start, example_space);
    std::vector<Program*> result;
    search(env, start, example_space, result, N, K);
    return result;
}

std::vector<Program *> OptimizedTermSolver::getTerms(const std::vector<PointExample *> &example_list, int N) {
    K_limit = std::max(int(example_list[0]->first.size()), config::KMaxBranchNum);
    for (int i = 1; i <= K_limit; ++i) {
        program_size_limit = 2 * N * i;
        auto result = getTerms(example_list, N, i);
        if (!result.empty() || util::checkTimeOut()) return result;
    }
    return {};
}

std::vector<Program *> OptimizedTermSolver::getTerms(const std::vector<PointExample *> &example_list) {

    if (example_list.size() == 0) return {new Program({}, new ConstSemantics(0))};
    while (1) {
        for (int N = 1; N <= example_list.size(); ++N) {
            auto result = getTerms(example_list, N);
            if (util::checkTimeOut()) return {};
            if (!result.empty()) {
                return result;
            }
        }
        releaseBound();
    }
}

void OptimizedTermSolver::clearCache() {
    info_map.clear();
    solved_sample.clear();
    plan_set.clear();
    visited_plan.clear();
}