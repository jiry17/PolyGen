#include "optimized_unifier.h"
#include "config.h"
#include "glog/logging.h"
#include "util.h"
#include <queue>
#include <set>

namespace {
    int clause_size_bound = 0;

    struct CmpPlan {
        std::vector<CmpInfo*> cmp_list;
        Bitset rem_example;
        CmpPlan(const std::vector<CmpInfo*>& _cmp_list, const Bitset& _rem): cmp_list(_cmp_list), rem_example(_rem) {}
        CmpPlan() = default;

        bool checkMinimal(Bitset init) {
            return true;
        }
        void print() const {
            std::cout << "rem " << rem_example.count() << std::endl;
            for (int i = 0; i < cmp_list.size(); ++i) {
                std::cout << cmp_list[i]->cmp->toString() << std::endl;
            }
        }
    };

    int operator < (const CmpPlan& cmp_1, const CmpPlan& cmp_2) {
        return cmp_1.cmp_list.size() < cmp_2.cmp_list.size() || (cmp_1.cmp_list.size() == cmp_2.cmp_list.size() && cmp_1.rem_example < cmp_2.rem_example);
    }

    struct ClausePlan {
        std::vector<int> cmp_id_list;
        Bitset P_info, N_info;
        ClausePlan(const std::vector<int>& _list, const Bitset& _P, const Bitset& _N): cmp_id_list(_list), P_info(_P), N_info(_N) {}
        bool cover(ClausePlan* plan) {
            return P_info.checkCover(plan->P_info) && plan->N_info.checkCover(N_info);
        }
    };

    int cmpCmpInfo(CmpInfo* info_1, CmpInfo* info_2) {
        return info_1->cmp->toString() < info_2->cmp->toString();
    }

    std::vector<CmpInfo*> insertNewCmp(const CmpPlan& plan, CmpInfo* info) {
        std::vector<CmpInfo*> cmp_list = plan.cmp_list;
        cmp_list.emplace_back();
        int pos = 0;
        while (pos + 1 < cmp_list.size() && cmpCmpInfo(cmp_list[pos], info)) ++pos;
        for (int i = int(cmp_list.size()) - 1; i > pos; --i) cmp_list[i] = cmp_list[i - 1];
        cmp_list[pos] = info;
        return cmp_list;
    }

    int visit_num = 0;

    ClausePlan* mergePlan(ClausePlan* l, ClausePlan* r) {
        std::vector<int> id_list = l->cmp_id_list;
        for (auto id: r->cmp_id_list) id_list.push_back(id);
        return new ClausePlan(id_list, l->P_info & r->P_info, l->N_info & r->N_info);
    }


    std::vector<ClausePlan*> reduceClause(std::vector<ClausePlan*>&& plan_list) {
        std::vector<std::vector<int>> feature_list;
        for (int i = 0; i < plan_list.size(); ++i) {
            auto* clause = plan_list[i];
            feature_list.push_back({-clause->P_info.count(), clause->N_info.count(), -int(clause->cmp_id_list.size()), i});
        }
        std::sort(feature_list.begin(), feature_list.end());
        std::vector<ClausePlan*> result;
        for (auto& feature: feature_list) {
            auto* clause = plan_list[feature[3]];
            bool is_duplicated = false;
            for (auto* pre_plan: result) {
                if (pre_plan->cover(clause)) {
                    is_duplicated = true;
                    break;
                }
            }
            if (!is_duplicated) result.push_back(clause); else delete clause;
        }
        return result;
    }

    std::vector<ClausePlan*> divideAndConquerNextClause(int l, int r, const std::vector<CmpInfo*>& cmp_info, const Bitset& rem_example, int limit) {
        std::vector<ClausePlan*> result;
        if (l == r) {
            auto* info = cmp_info[l];
            result.push_back(new ClausePlan({}, rem_example, Bitset(cmp_info[0]->N.size(), true)));
            auto rem_P = info->P & rem_example;
            if (rem_P.count() >= limit) {
                result.push_back(new ClausePlan({l}, rem_P, info->N));
            }
            return reduceClause(std::move(result));
        }
        int mid = l + r >> 1;
        auto l_result = divideAndConquerNextClause(l, mid, cmp_info, rem_example, limit);
        auto r_result = divideAndConquerNextClause(mid + 1, r, cmp_info, rem_example, limit);
        for (auto* l_plan: l_result) {
            for (auto* r_plan: r_result) {
                auto rem_P = l_plan->P_info & r_plan->P_info;
                if (rem_P.count() >= limit) {
                    result.push_back(mergePlan(l_plan, r_plan));
                }
            }
        }
        for (auto* plan: l_result) delete plan;
        for (auto* plan: r_result) delete plan;
        result = reduceClause(std::move(result));
        return result;
    }

    bool searchForSimpleClause(int id, const std::vector<Bitset>& prefix, const Bitset& now_n,
            const std::vector<CmpInfo*>& cmp_info, std::vector<CmpInfo*>& result) {
        if (id == -1) return true;
        if ((now_n & prefix[id]).count() == 0) {
            if (searchForSimpleClause(id - 1, prefix, now_n, cmp_info, result)) {
                return true;
            }
        }
        if (result.size() < clause_size_bound) {
            result.push_back(cmp_info[id]);
            if (searchForSimpleClause(id - 1, prefix, now_n & cmp_info[id]->N, cmp_info, result)) {
                return true;
            }
            result.pop_back();
        }
        return false;
    }

    CmpInfo* simplifyPlanGreedily(ClausePlan* plan, const std::vector<CmpInfo*>& cmp_info) {
        std::vector<CmpInfo*> used_info_list, result_info_list;
        for (int id: plan->cmp_id_list) {
            used_info_list.push_back(cmp_info[id]);
        }
        int n_n = plan->N_info.size();
        int p_n = plan->P_info.size();
        Bitset rem_n(n_n, true);
        while (rem_n.count()) {
            CmpInfo* best = nullptr;
            double best_cover_num = -1;
            for (auto* info: used_info_list) {
                double num = (rem_n.count() - (info->N & rem_n).count()) / double(info->cmp->size() + 1);
                if (num > best_cover_num) {
                    best_cover_num = num;
                    best = info;
                }
            }
            assert(best);
            result_info_list.push_back(best);
            rem_n = rem_n & best->N;
        }
        Bitset now_p = Bitset(p_n, true);
        std::vector<Program*> result_program;
        for (auto* cmp: result_info_list) {
            now_p = now_p & cmp->P;
            result_program.push_back(cmp->cmp);
        }
        delete plan;
        if (result_program.size() > clause_size_bound) {
            return nullptr;
        }
        return new CmpInfo(util::mergeClause(result_program), now_p, rem_n);
    }

    CmpInfo* simplifyPlan(ClausePlan* plan, const std::vector<CmpInfo*>& cmp_info) {
        int n = plan->cmp_id_list.size();
        std::vector<Bitset> prefix(n);
        int n_n = plan->N_info.size();
        int p_n = plan->P_info.size();
        prefix[0] = Bitset(n_n, true);
        std::vector<CmpInfo*> used_info_list;
        for (int i = 0; i < n; ++i) {
            used_info_list.push_back(cmp_info[plan->cmp_id_list[i]]);
        }
        for (int i = 1; i < n; ++i) {
            auto* info = used_info_list[i - 1];
            prefix[i] = prefix[i - 1] & info->N;
        }
        std::vector<CmpInfo*> result;
        if (!searchForSimpleClause(int(used_info_list.size()) - 1, prefix, prefix[0], used_info_list, result)) {
            return nullptr;
        }
        Bitset now_n = prefix[0];
        Bitset now_p = Bitset(p_n, true);
        std::vector<Program*> result_program;
        for (auto* cmp: result) {
            now_n = now_n & cmp->N;
            now_p = now_p & cmp->P;
            result_program.push_back(cmp->cmp);
        }
        delete plan;
        return new CmpInfo(util::mergeClause(result_program), now_p, now_n);
    }


    std::vector<CmpInfo*> reorder(const std::vector<CmpInfo*>& clause_list) {
        std::vector<std::pair<int, int>> info_list;
        for (int i = 0; i < clause_list.size(); ++i) {
            info_list.emplace_back(-clause_list[i]->P.count(), i);
        }
        std::sort(info_list.begin(), info_list.end());
        std::vector<std::pair<int, CmpInfo*>> result;
        for (auto& info_pair: info_list) {
            auto* info = clause_list[info_pair.second];
            bool is_duplicated = false;
            for (auto& pre_info: result) {
                if (pre_info.second->P.checkCover(info->P)) {
                    is_duplicated = true;
                }
            }
            if (!is_duplicated) result.emplace_back(info_pair.second, info);
        }
        std::sort(result.begin(), result.end());
        std::vector<CmpInfo*> res;
        for (auto& info_pair: result) {
            res.push_back(info_pair.second);
        }
        return res;
    }

    std::set<CmpPlan> visited_plan;

    Program* searchForCondition(const CmpPlan& plan, const std::vector<CmpInfo*>& cmp_info, int rem_or) {
        if (visited_plan.find(plan) != visited_plan.end()) return nullptr;
        visited_plan.insert(plan);
        if (rem_or == 0 || plan.rem_example.count() == 0) {
            assert(plan.rem_example.count() == 0);
            std::vector<Program*> clause_list;
            for (auto* cmp_info: plan.cmp_list) {
                clause_list.push_back(cmp_info->cmp);
            }
            return util::mergeCondition(clause_list);
        }
        int rem_number = plan.rem_example.count();
        int limit = (rem_number - 1) / rem_or + 1;
        std::vector<CmpInfo*> clause_list;
        for (auto* clause_plan: divideAndConquerNextClause(0, cmp_info.size() - 1, cmp_info, plan.rem_example, limit)) {
            if (clause_plan->N_info.count()) continue;
            auto* result = simplifyPlanGreedily(clause_plan, cmp_info);
            if (result) {
                clause_list.push_back(result);
            }
        }
        reorder(clause_list);
        for (auto* cmp: clause_list) {
            auto cmp_list = insertNewCmp(plan, cmp);
            auto* result = searchForCondition(CmpPlan(cmp_list, plan.rem_example & (~cmp->P)), cmp_info, rem_or - 1);
            if (result) return result;
        }
        return nullptr;
    }

    Program* searchForCondition(const std::vector<CmpInfo*>& cmp_info, int K) {
        visit_num = 0;
        int p_n = cmp_info[0]->P.size();
        Bitset full_positive(p_n, true);
        CmpPlan empty_plan({}, full_positive);
        visited_plan.clear();
        return searchForCondition(empty_plan, cmp_info, K);
    }

    std::string getFeature(const std::vector<CmpInfo*>& info_list) {
        std::vector<std::string> result;
        for (auto* info: info_list) {
            result.push_back(info->cmp->toString());
        }
        std::sort(result.begin(), result.end());
        std::string feature;
        for (auto& s: result) {
            feature += "@" + s;
        }
        return feature;
    }
}

Program * OptimizedUnifier::getCondition(const std::vector<PointExample *> &positive_example, const std::vector<PointExample *> &negative_example) {
    P = positive_example; N = negative_example;
    int current_size = 4;
    while (getCmpList(current_size).empty()) ++current_size;
    std::vector<std::vector<CmpInfo*>> cmp_lists;
    cmp_lists.emplace_back();

    auto extend_cmp_list = [&]() {
        std::string pre_feature = getFeature(cmp_lists[cmp_lists.size() - 1]);
        int same_count = 0;
        while (same_count <= 2) {
            auto cmp_list = getCmpList(current_size);
            ++current_size;
            if (getFeature(cmp_list) == pre_feature) {
                ++same_count;
                continue;
            }
            if (verifySolvable(cmp_list)) {
                cmp_lists.push_back(cmp_list);
                return;
            }
        }
        cmp_lists.push_back(cmp_lists[cmp_lists.size() - 1]);
    };

    int size_limit = 1, or_limit = 1;
    std::set<std::pair<int, int>> visited;
    while (1) {
        for (int or_num = 1; or_num <= or_limit; ++or_num) {
            for (int size = 1; size <= size_limit; ++size) {
                clause_size_bound = 2 * extra_list.size() * size;
                if (size == cmp_lists.size()) extend_cmp_list();
                if (visited.find(std::make_pair(or_num, size)) != visited.end()) continue;
                visited.insert(std::make_pair(or_num, size));
                auto* condition = searchForCondition(cmp_lists[size], or_num);
                if (condition) {
                    LOG(INFO) << "result " << visit_num << " " << condition->toString() << std::endl;
                    return condition;
                }
            }
        }
        or_limit = std::min(or_limit + 1, int(positive_example.size())); size_limit += 1;
    }
    assert(0);
}