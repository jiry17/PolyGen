//
// Created by pro on 2021/2/5.
//

#include "specification.h"

#include "util.h"
#include "semantics_factory.h"
#include "config.h"

namespace {
    z3::expr parseConsNode(Json::Value& node, z3::context& ctx, const std::vector<z3::expr>& var_values, std::string name = "result") {
        if (node["type"] == "const") {
            return Data(node).toZ3(ctx);
        } else if (node["type"] == "var") {
            return var_values[node["id"].asInt()];
        } else if (node["type"] == "op") {
            auto* semantics = semantics::getSemanticsFromName(node["op"].asString());
            std::vector<z3::expr> sub_list;
            for (auto& sub_node: node["sub"]) {
                sub_list.push_back(parseConsNode(sub_node, ctx, var_values, name));
            }
            auto* z3_semantics = dynamic_cast<Z3Semantics*>(semantics);
            return z3_semantics->getZ3Expr(std::move(sub_list), ctx, {});
        } else if (node["type"] == "func") {
            return util::getVariable(TINT, ctx, name);
        }
    }

    z3::expr parseConsNodeWithInp(Json::Value& node, const DataList& var_list, z3::expr& oup, z3::context& ctx) {
        if (node["type"] == "const") {
            return Data(node).toZ3(ctx);
        } else if (node["type"] == "var") {
            return var_list[node["id"].asInt()].toZ3(ctx);
        } else if (node["type"] == "op") {
            auto* semantics = semantics::getSemanticsFromName(node["op"].asString());
            std::vector<z3::expr> sub_list;
            for (auto& sub_node: node["sub"]) {
                sub_list.push_back(parseConsNodeWithInp(sub_node, var_list, oup, ctx));
            }
            auto* z3_semantics = dynamic_cast<Z3Semantics*>(semantics);
            return z3_semantics->getZ3Expr(std::move(sub_list), ctx, {});
        } else {
            return oup;
        }
    }

    void _checkPointWise(Json::Value& node, std::vector<Json::Value>& param) {
        if (node["type"] == "func") {
            if (param.empty()) {
                for (auto& sub: node["sub"]) {
                    param.push_back(sub);
                }
            } else {
                assert(param.size() == node["sub"].size());
                for (int i = 0; i < node["sub"].size(); ++i) {
                    assert(param[i].toStyledString() == node["sub"][i].toStyledString());
                }
            }
        } else if (node["type"] == "op") {
            for (auto& sub_node: node["sub"]) {
                _checkPointWise(sub_node, param);
            }
        } else if (node["type"] == "const" || node["type"] == "var") {
            return;
        } else {
            assert(0);
        }
    }

    Data _parseNodeValue(Json::Value& node, DataList& var_list) {
        if (node["type"] == "const") {
            return Data(node);
        } else if (node["type"] == "var") {
            return var_list[node["id"].asInt()];
        } else if (node["type"] == "op") {
            auto* semantics = semantics::getSemanticsFromName(node["op"].asString());
            DataList sub_list;
            for (auto& sub_node: node["sub"]) {
                sub_list.push_back(_parseNodeValue(sub_node, var_list));
            }
            return semantics->run(std::move(sub_list), nullptr);
        } else {
            assert(0);
        }
    }
}

void Specification::checkPointWise() {
    for (auto& cons: cons_root) {
        _checkPointWise(cons, param_list);
    }
}

Specification::Specification(Json::Value root) {
    return_type = util::string2Type(root["return"].asString());
    auto param_root = root["param"];
    for (auto& param_node: param_root) {
        auto type = param_node["type"].asString();
        param_types.push_back(util::string2Type(type));
    }
    if (param_types.size() <= 5) config::KTermIntMax = 10, config::KOrLimit = 4;
    auto var_root = root["param"];
    for (auto& var_node: param_root) {
        auto type = var_node["type"].asString();
        var_types.push_back(util::string2Type(type));
    }
    cons_root = root["cons"];
    auto const_root = root["consts"];
    global::int_consts.clear();
    for (auto& value: const_root) {
        global::int_consts.push_back(value.asInt());
    }
    checkPointWise();

    for (int i = 0; i < var_types.size(); ++i) {
        std::string name = util::type2String(var_types[i]) + "@" + std::to_string(i);
        z3_vars.push_back(util::getVariable(var_types[i], ctx, name));
    }
    for (auto& param_node: param_list) {
        z3_params.push_back(parseConsNode(param_node, ctx, z3_vars));
    }
}

z3::expr Specification::buildCons(std::string name) {
    z3::expr_vector z3_cons_list(ctx);
    for (auto cons_node: cons_root) {
        z3_cons_list.push_back(parseConsNode(cons_node, ctx, z3_vars, name));
    }
    return z3::mk_and(z3_cons_list);
}

z3::expr Specification::buildProgramCons(const std::vector<Program *> &candidate_programs) {
    z3::expr_vector z3_list(ctx);
    z3::expr res = util::getVariable(TINT, ctx, "result");
    for (auto* program: candidate_programs) {
        z3_list.push_back(res == program->getZ3Expr(ctx, z3_params));
    }
    return z3::mk_or(z3_list);
}

PointExample * Specification::buildExample(const DataList &inp) {
    z3::solver S(ctx);
    S.add(buildCons());
    for (int i = 0; i < z3_params.size(); ++i) {
        S.add(z3_params[i] == ctx.int_val(inp[i].getInt()));
    }
    assert(S.check() == z3::sat);
    auto res = util::getVariable(TINT, ctx, "result");
    auto oup = util::getVariableData(TINT, res, S.get_model());
    S.add(res != oup.getInt());
    if (S.check() == z3::unsat) {
        return new PointExample(inp, oup);
    } else {
        for (int i = 0; i < inp.size(); ++i) std::cout << inp[i].getInt() << " "; std::cout << std::endl;
        std::cout << oup.getInt() << std::endl;
        std::cout << S.get_model() << std::endl;
        return nullptr;
    }
}

z3::model Specification::getModel(z3::solver &S, bool is_random) {
    if (!is_random) return S.get_model();
    auto model = S.get_model();
    auto vars = z3_vars;
    std::random_shuffle(vars.begin(), vars.end());
    for (auto& var: vars) {
        int bound = util::getRandomInt();
        if (std::rand() & 1) {
            S.add(var >= bound);
        } else {
            S.add(var <= bound);
        }
        if (S.check() == z3::sat) {
            model = S.get_model();
        } else break;
    }
    return model;
}

PointExample * Specification::verify(Program *program, bool is_random) {
    z3::solver S(ctx);
    S.add(!buildCons());
    S.add(buildProgramCons({program}));
    if (S.check() == z3::unsat) {
        return nullptr;
    }
    return getExampleFromSolver(S, is_random);
}

PointExample * Specification::getExampleFromSolver(z3::solver &S, bool is_random) {
    auto model = getModel(S, is_random);
    DataList var_inp;
    for (int i = 0; i < z3_vars.size(); ++i) {
        var_inp.push_back(util::getVariableData(var_types[i], z3_vars[i], model));
    }
    DataList inp;
    for (auto& param_node: param_list) {
        inp.push_back(_parseNodeValue(param_node, var_inp));
    }
    auto* counter_example = buildExample(inp);
    assert(counter_example);
    return counter_example;
}

z3::expr Specification::buildDefinition() {
    z3::expr cons = buildCons();
    z3::expr next_cons = buildCons("result2");
    z3::expr v1 = util::getVariable(TINT, ctx, "result"), v2 = util::getVariable(TINT, ctx, "result2");
    z3::expr differ = (v1 == v2 + 1);
    return cons && differ && !next_cons;
}

PointExample * Specification::verifyConditionNegative(Program *term, Program *condition, const std::vector<Program*>& previous_conditions, bool is_random) {
    z3::solver S(ctx);
    S.add(!buildCons());
    S.add(buildProgramCons({term}));
    S.add(condition->getZ3Expr(ctx, z3_params));
    for (auto* pre_cond: previous_conditions) S.add(!condition->getZ3Expr(ctx, z3_params));
    if (S.check() == z3::unsat) return nullptr;
    return getExampleFromSolver(S, is_random);
}

PointExample * Specification::verifyConditionPositive(Program *term, const std::vector<Program *> &remain_terms, Program *condition, const std::vector<Program*>& previous_conditions, bool is_random) {
    z3::solver S(ctx);
    S.add(buildDefinition());
    S.add(buildProgramCons({term}));
    S.add(!buildProgramCons(remain_terms));
    S.add(!condition->getZ3Expr(ctx, z3_params));
    for (auto* pre_cond: previous_conditions) S.add(!condition->getZ3Expr(ctx, z3_params));
    if (S.check() == z3::unsat) return nullptr;
    return getExampleFromSolver(S, is_random);
}

PointExample * Specification::verifyProgramList(const std::vector<Program*> &program, bool is_random) {
    z3::solver S(ctx);
    S.add(buildDefinition());
    S.add(!buildProgramCons(program));
    // std::cout << S << std::endl;
    if (S.check() == z3::unsat) {
        return nullptr;
    }
    return getExampleFromSolver(S, is_random);
}

void Specification::print() const {
    std::cout << "Return type: " << util::type2String(return_type) << std::endl;
    std::cout << "Var list:";
    for (auto& type: var_types) {
        std::cout << " " << util::type2String(type);
    }
    std::cout << std::endl;
    std::cout << "Param list:";
    for (auto& type: param_types) {
        std::cout << " " << util::type2String(type);
    }
    std::cout << std::endl;
    std::cout << "Params:" << std::endl;
    for (auto& node: param_list) {
        std::cout << "  " << node.toStyledString() << std::endl;
    }
    std::cout << "Constraint:" << std::endl;
    for (auto& cons: cons_root) {
        std::cout << "  " << cons.toStyledString() << std::endl;
    }
}