#include <iostream>

#include "program.h"
#include "config.h"

Data Program::run(GlobalInfo *inp) {
    DataList sub_expr;
    for (auto* sub_program: sub_list) {
        auto sub_result = std::move(sub_program->run(inp));
        if (sub_result.getType() == TNONE) return std::move(sub_result);
        sub_expr.emplace_back(std::move(sub_result));
    }
    if (sub_expr.size() == semantics->inp_type_list.size()) {
        return semantics->run(std::move(sub_expr), inp);
    } else {
        return semantics::curry(semantics, sub_expr);
    }
}

Data Program::run(const DataList &inp) {
    auto* param_info = new ParamInfo(inp);
    Data result = run(param_info);
    if (result.getType() == TNONE) {
        if (semantics->oup_type == TINT) return config::KDefaultValue;
        else return new BoolValue(false);
    }
    delete param_info;
    return std::move(result);
}

std::string Program::toString() {
    if (sub_list.size() == 0) return semantics->name;
    std::string result = "(" + semantics->name;
    for (auto* sub_expr: sub_list) {
        result += " " + sub_expr->toString();
    }
    return result + ")";
}

void Program::print() {
    std::cout << toString() << std::endl;
}

int Program::size() {
    assert(semantics);
    int ans = (semantics->name != "int");
    for (auto* sub_program: sub_list) {
        ans += sub_program->size();
    }
    return ans;
}

z3::expr Program::getZ3Expr(z3::context &ctx, const std::vector<z3::expr> &param_list) {
    std::vector<z3::expr> sub_result;
    for (auto* sub_program: sub_list) {
        sub_result.push_back(sub_program->getZ3Expr(ctx, param_list));
    }
    auto* z3_semantics = dynamic_cast<Z3Semantics*>(semantics);
    assert(z3_semantics);
    return z3_semantics->getZ3Expr(std::move(sub_result), ctx, param_list);
}