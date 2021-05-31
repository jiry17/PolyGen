//
// Created by pro on 2021/1/8.
//

#include "term_solver.h"
#include "semantics_factory.h"

#include "glog/logging.h"

namespace {
    const double EPS = 1e-6;

    Program* mergeParamProduct(const std::vector<int>& current) {
        Program* result = nullptr;
        for (auto& id: current) {
            auto* param = new Program({}, new ParamSemantics(id, TINT));
            if (!result) result = param;
            else result = new Program({result, param}, semantics::getSemanticsFromName("*"));
        }
        return result;
    }

    void collectAllProduct(int pos, const std::vector<int>& l, int size_limit, std::vector<int>& current, std::vector<Program*>& result) {
        if (pos == l.size()) {
            if (!current.empty()) result.push_back(mergeParamProduct(current));
            return;
        }
        collectAllProduct(pos + 1, l, size_limit, current, result);
        std::vector<int> pre = current;
        while (current.size() < size_limit) {
            current.push_back(l[pos]);
            collectAllProduct(pos + 1, l, size_limit, current, result);
        }
        current = pre;
    }

    Data convertDefault(const Data& k) {
        if (abs(k.getInt()) == config::KDefaultValue) return k.getInt() < 0 ? -100:100;
        return k;
    }
}

Program* TermSolver::merge(const std::vector<Program*>& atom_list, const std::vector<int>& param_list) {
    assert(atom_list.size() + 1 == param_list.size());
    int n = atom_list.size();
    Program* result = nullptr;
    for (int i = 0; i < n; ++i) {
        if (param_list[i]) {
            auto* current_atom = atom_list[i];
            if (param_list[i] != 1) {
                auto* param = new Program({}, new ConstSemantics(param_list[i]));
                current_atom = new Program({param, current_atom}, semantics::getSemanticsFromName("*"));
            }
            if (result) result = new Program({result, current_atom}, semantics::getSemanticsFromName("+"));
            else result = current_atom;
        }
    }
    if (param_list[n]) {
        auto* const_atom = new Program({}, new ConstSemantics(param_list[n]));
        if (result) result = new Program({result, const_atom}, semantics::getSemanticsFromName("+"));
        else result = const_atom;
    }
    if (!result) result = new Program({}, new ConstSemantics(0));
    return result;
}

std::vector<Program *> TermSolver::getTermList(int size_limit) {
    std::vector<int> current_id;
    std::vector<Program*> result;
    collectAllProduct(0, param_list, size_limit, current_id, result);
    return result;
}

int TermSolver::solveCLIAParam(const std::vector<PointExample *> &pointer_list, std::vector<int> &result, bool is_require_single) {
    std::vector<PointExample> example_list;
    for (auto* pointer: pointer_list) {
        example_list.emplace_back(*pointer);
    }
    return solveCLIAParam(example_list, result, is_require_single);
}

// 0: no solution, 1: single solution, 2: multi solution
int TermSolver::solveCLIAParam(const std::vector<PointExample> &example_list, std::vector<int> &result, bool is_require_single) {
    std::vector<std::vector<double>> x;
    int n = example_list[0].first.size() + 2;
    int m = example_list.size();
    for (auto& example: example_list) {
        std::vector<double> equation(n);
        equation[0] = example.second.getInt();
        for (int i = 0; i < example.first.size(); ++i) {
            equation[i + 1] = example.first[i].getInt();
        }
        equation[n - 1] = 1;
        x.emplace_back(std::move(equation));
    }
    std::vector<double> y(n - 1);
    int now = 0;
    bool is_single_solution = false;
    std::vector<int> ind;
    for (int i = 1 ; i < n; ++i) {
        int r = -1;
        for (int j = now; j < x.size(); ++j) {
            if (fabs(x[j][i]) > EPS) {
                r = j; break;
            }
        }
        if (r == -1) {
            if (is_require_single) return 2;
            y[i - 1] = 0.0; continue;
        }
        for (int j = 0; j < n; ++j) std::swap(x[now][j], x[r][j]);
        double weight = x[now][i];
        for (int j = 0; j < n; ++j) x[now][j] /= weight;
        for (int j = 0; j < x.size(); ++j) {
            if (j == now || fabs(x[j][i]) < EPS) continue;
            double w = x[j][i];
            for (int k = 0; k < n; ++k) x[j][k] -= w * x[now][k];
        }
        now++; ind.push_back(i);
    }
    for (int i = now; i < x.size(); ++i) {
        if (fabs(x[i][0]) > EPS) return false;
    }
    for (int i = 0; i < ind.size(); ++i) {
        y[ind[i] - 1] = x[i][0];
    }

    int non_zero_num = 0;
    result.clear();
    for (int i = 0; i < y.size(); ++i) {
        int r = round(y[i]);
        if (fabs(r - y[i]) > EPS || r < config::KIntMin || r > config::KIntMax) return false;
        result.push_back(r);
        if (r > 0) ++non_zero_num;
    }
    return non_zero_num <= config::KMaxBranchNum;
}

std::vector<Program*> GaussTermSolver::getTerms(const std::vector<PointExample*>& example_list) {
    for (int atom_size = 1;; ++atom_size) {
        LOG(INFO) << "term " << atom_size << std::endl;
        std::vector<Program *> atom_list = getTermList(atom_size);
        int branch_num = 1;
        std::vector<PointExample*> rem_example = example_list;
        std::vector<Program *> term_list;
        for (int term_id = 1; term_id <= branch_num && branch_num <= config::KMaxBranchNum; ++term_id) {
            Program *term = nullptr;
            int best_pass_num = 0;
            int limit = (rem_example.size() - 1) / (branch_num - term_id + 1);
            for (int turn_id = 1; turn_id <= config::KTermSolveTurns; ++turn_id) {
                std::vector<PointExample> sampled_sample_list;
                std::random_shuffle(rem_example.begin(), rem_example.end());
                for (int i = 0; i < rem_example.size() && i <= atom_list.size(); ++i) {
                    DataList inp;
                    for (auto *atom: atom_list) {
                        inp.push_back(convertDefault(atom->run(rem_example[i]->first)));
                    }
                    sampled_sample_list.emplace_back(std::move(inp), convertDefault(rem_example[i]->second));
                }
                std::vector<int> param_list;
                bool is_success = solveCLIAParam(sampled_sample_list, param_list);
                if (!is_success) continue;
                auto *candidate = merge(atom_list, param_list);

                int pass_num = 0;
                for (auto* example: rem_example) {
                    if (candidate->run(example->first) == example->second) {
                        ++pass_num;
                    }
                }
                if (pass_num > best_pass_num) {
                    best_pass_num = pass_num;
                    term = candidate;
                    if (best_pass_num > limit) break;
                }
            }
            if (!term) break;
            while (branch_num <= config::KMaxBranchNum && (rem_example.size() - 1) / (branch_num - term_id + 1) >= best_pass_num) {
                ++branch_num;
            }
            LOG(INFO) << term->toString() << " " << best_pass_num << " " << rem_example.size() << " " << branch_num << std::endl;
            if (branch_num > config::KMaxBranchNum) break;
            term_list.push_back(term);
            int now = 0;
            for (auto* example: rem_example) {
                if (term->run(example->first) != example->second) {
                    rem_example[now++] = example;
                }
            }
            rem_example.resize(now);
        }
        if (rem_example.empty()) return term_list;
    }
}