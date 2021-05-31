#include "enumerate_term_solver.h"

#include "enumerator.h"
#include "semantics_factory.h"
#include "grammar.h"
std::vector<Program *> EnumerateTermSolver::getTerms(const std::vector<PointExample *> &example_list) {
    auto* atom = new NonTerminal("param", TINT);

    int param_num = example_list[0]->first.size();

    std::vector<int> param_list;
    for (int i = 0; i < param_num; ++i) param_list.push_back(i);

    for (auto& param_id: param_list) {
        atom->rule_list.push_back(new Rule(new ParamSemantics(param_id, TINT), {}));
    }
    for (int int_const: global::int_consts) {
        atom->rule_list.push_back(new Rule(new ConstSemantics(int_const), {}));
    }
    auto* expr = new NonTerminal("expr", TINT);
    expr->rule_list.push_back(new Rule(semantics::getSemanticsFromName("int"), {atom}));
    expr->rule_list.push_back(new Rule(semantics::getSemanticsFromName("+"), {expr, expr}));
    expr->rule_list.push_back(new Rule(semantics::getSemanticsFromName("-"), {expr, expr}));
    auto* grammar = new Grammar(expr, {atom, expr});
    auto* enumerator = new Enumerator(grammar, config::KDefaultValue);
    std::cout << example_list.size() << std::endl;
    auto candidates = enumerator->enumerateProgram(example_list);

    //for(auto i : candidates) std::cout << i->toString() <<std::endl;
    return candidates;
}