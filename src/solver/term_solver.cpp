//
// Created by pro on 2021/1/8.
//

#include "term_solver.h"
#include <set>

namespace {
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
}

int solver::getOptimalAssignment(GRBEnv& env, const std::vector<PointExample*> &example_list, std::vector<int>& result) {
    // TODO: Special treatments for INF and -INF
    int n = example_list[0]->first.size();
    GRBModel model = GRBModel(env);
    model.set(GRB_IntParam_OutputFlag, 0);
    std::vector<GRBVar> var_list;
    std::vector<GRBVar> bound_list;
    for (int i = 0; i <= n; ++i) {
        std::string name_var = "var" + std::to_string(i);
        std::string name_bound = "bound" + std::to_string(i);
        int bound = i < n ? config::KTermIntMax : getConstBound();
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
    result.clear();
    for (auto var: var_list) {
        result.push_back(convertToInt(var.get(GRB_DoubleAttr_X)));
    }
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