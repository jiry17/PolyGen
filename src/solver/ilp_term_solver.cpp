#include "ilp_term_solver.h"
#include <set>

namespace {
    Data convertDefault(const Data& k) {
        if (abs(k.getInt()) == config::KDefaultValue) return k.getInt() < 0 ? -100:100;
        return k;
    }

    std::set<std::vector<int> > S;

    double convertToInt(double x) {
        int w = int(x);
        while (w < x - 0.5) ++w;
        while (w > x + 0.5) --w;
        return w;
    }

    int getConstBound() {
        int num = config::KTermIntMax;
        for (auto& int_const: global::int_consts) {
            num = std::max(num, int_const);
        }
        return num;
    }

    std::set<std::vector<int> > forbid_assignment;
}

int solver::getOptimalAssignment(GRBEnv& env, const std::vector<PointExample*> &example_list, std::vector<int>& result, const std::vector<int>& initial_solution) {
    // TODO: Special treatments for INF and -INF
    int n = example_list[0]->first.size();
    /*int status = solveCLIAParam(example_list, result, true);
    if (status == 0) return 0;
    if (status == 1) {
        for (int i = 0; i <= n; i++) if (abs(result[i]) > config::KTermIntMax) return 0;
        return status;
    }*/
    GRBModel model = GRBModel(env);
    model.set(GRB_IntParam_OutputFlag, 0);
    std::vector<GRBVar> var_list;
    std::vector<GRBVar> bound_list;
    for (int i = 0; i <= n; ++i) {
        std::string name_var = "var" + std::to_string(i);
        std::string name_bound = "bound" + std::to_string(i);
        int bound = i < n ? config::KTermIntMax : getConstBound();
        // std::cout << bound << std::endl;
        var_list.push_back(model.addVar(-bound, bound, 0.0, GRB_INTEGER, name_var));
        if (i == n) {
            bound_list.push_back(model.addVar(0, 1, 0.0, GRB_INTEGER, name_bound));
            model.addConstr(var_list[i] <= bound * bound_list[i], "rbound" + std::to_string(i));
            model.addConstr(var_list[i] >= -bound * bound_list[i], "lbound" + std::to_string(i));
        } else {
            bound_list.push_back(model.addVar(0, bound, 0.0, GRB_INTEGER, name_bound));
            model.addConstr(var_list[i] <= bound_list[i], "rbound" + std::to_string(i));
            model.addConstr(var_list[i] >= -bound_list[i], "lbound" + std::to_string(i));
        }
    }
    if (!initial_solution.empty()) {
        for (int i = 0; i < initial_solution.size(); ++i) {
            var_list[i].set(GRB_DoubleAttr_Start, initial_solution[i]);
            bound_list[i].set(GRB_DoubleAttr_Start, !!initial_solution[i]);
        }
    }
    int id = 0;
    for (auto& example: example_list) {
        GRBLinExpr expr = var_list[n];
        for (int i = 0; i < n; ++i) expr += example->first[i].getInt() * var_list[i];
        model.addConstr(expr == example->second.getInt(), "cons" + std::to_string(id++));
    }
    GRBLinExpr target = 0;
    for (auto bound_var: bound_list) {
        target += bound_var;
    }
    model.setObjective(target, GRB_MINIMIZE);
    model.optimize();
    int status = model.get(GRB_IntAttr_Status);
    if (status == GRB_INFEASIBLE) {
        result.clear();
        return 0;
    }
    // std::cout << "find solution" << std::endl;
    result.clear();
    for (auto var: var_list) {
        result.push_back(convertToInt(var.get(GRB_DoubleAttr_X)));
    }
    // for (auto now: result) std::cout << now << " "; std::cout << std::endl;
#ifdef DEBUG
    for (auto& example: example_list) {
        int partial_oup = result[n];
        for (int i = 0; i < n; ++i) {
            partial_oup += result[i] * example->first[i].getInt();
        }
        if (partial_oup != example->second.getInt()) {
            std::cout << partial_oup << " " << example->second.getInt() << std::endl;
            std::cout << std::endl;
            for (int i = 0; i <= n; ++i) std::cout << result[i] << " "; std::cout << std::endl;
            assert(partial_oup == example->second.getInt());
        }
    }
#endif
    return 1;
}

int ILPTermSolver::getConfidence(const std::vector<int> &assignment, const std::vector<PointExample*>& example_list) {
    int n = example_list[0]->first.size();
    int confidence = 0;
    std::vector<int> new_assignment;
    for (int turn_id = 0; turn_id < config::KTermConfidenceNum; ++turn_id) {
        std::vector<PointExample*> selected_list;
        for (int i = 0; i <= n; ++i) {
            selected_list.push_back(example_list[rand() % example_list.size()]);
        }
        assert(solver::getOptimalAssignment(env, selected_list, new_assignment));
        if (new_assignment == assignment) {
            ++confidence;
        }
    }
    return confidence;
}

std::vector<TermConfig*> ILPTermSolver::getPossibleTerm(const std::vector<PointExample*> &example_list, int rem_size) {
    std::vector<TermConfig*> result;
    std::vector<int> assignment;
    S = forbid_assignment;
    int n = example_list[0]->first.size();
    int m = example_list.size();
    int lim = (m - 1) / rem_size + 1;
    for (int turn_id = 0; turn_id < config::KTermSolveTurns; ++turn_id) {
        std::vector<PointExample*> selected_example;
        for (int i = 0; i <= n; ++i) {
            selected_example.push_back(example_list[rand() % example_list.size()]);
        }
        int status = solver::getOptimalAssignment(env, selected_example, assignment);
        if (status == 0) continue;
        if (S.find(assignment) == S.end())  {
            S.insert(assignment);
            std::vector<PointExample*> rem_list;
            int count = 0;
            for (auto& example: example_list) {
                int now = assignment[n];
                for (int i = 0; i < n; ++i) now += assignment[i] * example->first[i].getInt();
                if (now == example->second.getInt()) {
                    ++count;
                } else {
                    rem_list.push_back(example);
                }
            }
            if (count >= lim) {
                result.push_back(new TermConfig(std::move(assignment), count, std::move(rem_list)));
            }
        }
    }
    return result;
}

void ILPTermSolver::searchForOptimal(const std::vector<PointExample *> &example_list, int rem, int size_sum, std::vector<std::vector<int> > &solution) {
    if (size_sum + rem >= optimal_size) return;
    if (rem == 0) {
        assert(example_list.size() == 0);
        optimal_size = size_sum;
        optimal_solution = solution;
        return;
    }
    auto candidate_list = getPossibleTerm(example_list, rem);
    std::sort(candidate_list.begin(), candidate_list.end(),
            [](TermConfig* k1, TermConfig* k2) {return k1->size < k2->size;});
    int pre_max = 0;
    std::vector<std::vector<int> > forbid_list;
    for (auto* candidate: candidate_list) {
        if (size_sum + rem - 1 + candidate->size < optimal_size && candidate->cover_num > pre_max) {
            pre_max = candidate->cover_num;
            solution.push_back(candidate->assignment);
            searchForOptimal(candidate->example_list, rem - 1, size_sum + candidate->size, solution);
            solution.pop_back();
            forbid_assignment.insert(candidate->assignment);
            forbid_list.push_back(candidate->assignment);
        }
    }
    for (auto& candidate: forbid_list) {
        forbid_assignment.erase(candidate);
    }
    for (auto* candidate: candidate_list) {
        delete candidate;
    }
}


std::vector<Program *> ILPTermSolver::getTerms(const std::vector<PointExample *> &example_list) {
    optimal_solution.clear();
    optimal_size = 1000000000;
    int atom_size = 1;
    auto term_list = getTermList(atom_size);
    atom_list = term_list;
    std::vector<PointExample*> term_example_list;
    for (auto* example: example_list) {
        DataList inp;
        for (auto* term: term_list) {
            inp.push_back(convertDefault(term->run(example->first)));
        }
        term_example_list.push_back(new PointExample(std::move(inp), convertDefault(example->second)));
    }
    for (int branch_num = 1; branch_num <= config::KMaxBranchNum; ++branch_num) {
        std::cout << "barnch_num " << branch_num << std::endl;
        std::vector<std::vector<int> > solution;
        searchForOptimal(term_example_list, branch_num, 0, solution);
        if (!optimal_solution.empty()) break;
    }
    std::vector<Program*> result;
    for (auto& assignment: optimal_solution) {
        result.push_back(merge(term_list, assignment));
    }
    for (auto* example: term_example_list) delete example;
    return std::move(result);
}