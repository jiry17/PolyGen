#include "unifier.h"
#include "enumerator.h"
#include "semantics_factory.h"
#include "grammar.h"
#include <set>

namespace {
    Program* rewriteWithExtra(Program* program, const std::vector<Program*>& extra_list) {
        auto* param = dynamic_cast<ParamSemantics*>(program->semantics);
        if (param) {
            int id = param->getId();
            if (id < 0) return extra_list[-id - 1]->copy();
            return program->copy();
        }
        std::vector<Program*> sub_list;
        for (auto* sub: program->sub_list) {
            sub_list.push_back(rewriteWithExtra(sub, extra_list));
        }
        if (program->semantics->name == "int") return sub_list[0];
        return new Program(sub_list, program->semantics);
    }
}

Program* util::mergeClause(const std::vector<Program*>& cmp_list) {
    if (cmp_list.empty()) return new Program({}, new ConstSemantics(new BoolValue(true)));
    auto* clause = cmp_list[0]->copy();
    for (int i = 1; i < cmp_list.size(); ++i) {
        clause = new Program({clause, cmp_list[i]->copy()}, semantics::getSemanticsFromName("and"));
    }
    return clause;
}

Program* util::mergeCondition(const std::vector<Program*>& clause_list) {
    Program* result = nullptr;
    if (clause_list.empty()) {
        return new Program({}, new ConstSemantics(new BoolValue(false)));
    }
    for (auto* program: clause_list) {
        if (!result) result = program;
        else result = new Program({result, program}, semantics::getSemanticsFromName("or"));
    }
    return result;
}

std::vector<CmpInfo *> Unifier::excludeDuplicated(const std::vector<Program *> &program_list) {
    std::vector<std::pair<Bitset, Bitset>> info_list;
    std::vector<CmpInfo*> result;
    std::vector<bool> is_duplicate;
    for (auto* program: program_list) {
        Bitset N_status, P_status;
        for (auto* example: P) P_status.append(program->run(example->first).getBool());
        for (auto* example: N) N_status.append(program->run(example->first).getBool());
        info_list.emplace_back(P_status, N_status);
        is_duplicate.push_back(false);
    }
    for (int i = int(program_list.size()) - 1; i >= 0; --i) {
        for (int j = 0; j < program_list.size(); ++j) {
            if (j != i && !is_duplicate[j] && info_list[j].first.checkCover(info_list[i].first) && info_list[i].second.checkCover(info_list[j].second)) {
                is_duplicate[i] = true;
                break;
            }
        }
    }
    for (int i = 0; i < program_list.size(); ++i) {
        if (!is_duplicate[i]) {
            result.push_back(new CmpInfo(program_list[i], info_list[i].first, info_list[i].second));
        }
    }
    return result;
}

bool Unifier::verifySolvable(const std::vector<CmpInfo *> &cmp_list) {
    if (cmp_list.empty()) return false;
    int p_n = cmp_list[0]->P.size();
    int n_n = cmp_list[0]->N.size();
    for (int i = 0; i < p_n; ++i) {
        Bitset n_status(n_n, true);
        for (auto* cmp_info: cmp_list) {
            if (cmp_info->P[i]) {
                n_status = n_status & cmp_info->N;
            }
        }
        if (n_status.count()) {
            return false;
        }
    }
    return true;
}

std::vector<CmpInfo *> Unifier::getCmpList(int cmp_size) {
    auto* cmp = new NonTerminal("cmp", TBOOL);
    auto* atom = new NonTerminal("param", TINT);
    for (auto& param_id: param_list) {
        atom->rule_list.push_back(new Rule(new ParamSemantics(param_id, TINT), {}));
    }
    for (int i = 0; i < extra_list.size(); ++i) {
        atom->rule_list.push_back(new Rule(new ParamSemantics(-i - 1, TINT), {}));
    }
    for (int int_const: global::int_consts) {
        atom->rule_list.push_back(new Rule(new ConstSemantics(int_const), {}));
    }
    auto* expr = new NonTerminal("expr", TINT);
    expr->rule_list.push_back(new Rule(semantics::getSemanticsFromName("int"), {atom}));
    expr->rule_list.push_back(new Rule(semantics::getSemanticsFromName("+"), {expr, atom}));
    cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("<="), {expr, expr}));
    cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("<"), {expr, expr}));
    auto* grammar = new Grammar(cmp, {cmp, atom, expr});
    auto* enumerator = new Enumerator(grammar, cmp_size);
    auto candidates = enumerator->enumerateProgram(config::KDefaultValue);
    for (int i = 0; i < candidates.size(); ++i) {
        candidates[i] = rewriteWithExtra(candidates[i], extra_list);
    }
    return excludeDuplicated(candidates);
}