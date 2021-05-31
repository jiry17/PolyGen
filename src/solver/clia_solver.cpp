#include "clia_solver.h"
#include "semantics_factory.h"
#include "util.h"
#include "glog/logging.h"
#include "recorder.h"
#include "grammar.h"
#include "semantics.h"
#include "enumerator.h"
#include <set>

namespace {
    Program* mergeIte(std::vector<Program*>& term_list, std::vector<Program*>& condition_list) {
        int n = term_list.size();
        auto* program = term_list[n - 1];
        for (int i = n - 2; i >= 0; --i) {
            program = new Program({condition_list[i], term_list[i], program}, semantics::getSemanticsFromName("ite"));
        }
        return program;
    }
    
    PointExample* verify_by_example(const std::vector<PointExample*>& example_space, Program * prog){
        for(auto pt : example_space){
            if (prog->run(pt->first) != pt->second) return pt;
        }
        return nullptr;
    }

}

CLIASolver::CLIASolver(TermSolver *_term_solver, Unifier* _unifier):
    term_solver(_term_solver), unifier(_unifier){
}

/*Program* CLIASolver::synthesize(Specification *spec) {
    std::vector<PointExample*> example_space;
    int param_num = spec->param_list.size();
    // Insert random example
    for (int i = 0; i < 32; ++i) {
        DataList inp;
        for (int j = 0; j < param_num; ++j) inp.push_back(util::getRandomInt());
        auto* example = spec->buildExample(inp);
        if (example) example_space.push_back(example);
    }
    // Insert random solved example
    for (int i = 0; i < 32; ++i) {
        auto* dummy = new Program({}, new ConstSemantics(0));
        auto* example = spec->verify(dummy, true);
        if (example) example_space.push_back(example);
    }
    // Solve Term
    std::vector<Program*> term_list;
    while (true) {
        term_list = term_solver->getTerms(example_space);
        std::cout << "term" << std::endl;
        for (auto* t: term_list) std::cout << t->toString() << std::endl;
        if (!spec->verifyProgramList(term_list, false)) break;
        for (int i = 1; i <= 1; ++i) {
            auto *example = spec->verifyProgramList(term_list, true);
            std::cout << *example << std::endl;
            if (example) example_space.push_back(example);
            else break;
        }
    }
    // Solve Condition
    std::vector<Program*> condition_list;
    std::vector<int> param_list;
    for (int i = 0; i < spec->param_list.size(); ++i) param_list.push_back(i);
    unifier->param_list = param_list;
    unifier->extra_list = term_list;

    for (int i = 0; i + 1 < term_list.size(); ++i) {
        std::vector<PointExample*> positive_list, negative_list, mid_list;
        auto* current_term = term_list[i];
        for (auto* current_example: example_space) {
            if (current_term->run(current_example->first) == current_example->second) {
                bool is_mid = false;
                for (int j = i + 1; j < term_list.size(); ++j) {
                    if (term_list[j]->run(current_example->first) == current_example->second) {
                        is_mid = true;
                        break;
                    }
                }
                if (is_mid) {
                    mid_list.push_back(current_example);
                } else positive_list.push_back(current_example);
            } else negative_list.push_back(current_example);
        }
        Program* condition = nullptr;
        std::vector<Program*> remain_terms;
        std::vector<Program*> previous_conditions;
        for (int j = i + 1; j < term_list.size(); ++j) remain_terms.push_back(term_list[j]);
        for (int j = 0; j < i; ++j) previous_conditions.push_back(condition_list[j]);
        while (true) {
            condition = unifier->getCondition(positive_list, negative_list);
            auto* counter_example = spec->verifyConditionNegative(current_term, condition, previous_conditions, true);
            if (counter_example) {
                negative_list.push_back(counter_example);
                continue;
            }
            counter_example = spec->verifyConditionPositive(current_term, remain_terms, condition, previous_conditions, true);
            if (counter_example) {
                positive_list.push_back(counter_example);
                continue;
            }
            break;
        }
        condition_list.push_back(condition);
        example_space = negative_list;
        for (auto* example: mid_list) {
            if (!(condition->run(example->first).getBool())) example_space.push_back(example);
        }
    }

    auto* result = mergeIte(term_list, condition_list);
    assert(!spec->verify(result, false));
    return result;
}*/

std::pair<int, Program *> CLIASolver::dealWithRandom(const std::vector<PointExample *> &example_space) {
    std::vector<PointExample*> used_examples;
    std::vector<Program*> term_list;

    auto verify_programs = [=](const std::vector<Program*>& terms) -> int{
        for (int i = 0; i < example_space.size(); ++i) {
            bool is_satisfied = false;
            for (auto* term: terms) {
                if (term->run(example_space[i]->first) == example_space[i]->second) {
                    is_satisfied = true;
                    break;
                }
            }
            if (!is_satisfied) return i;
        }
        return -1;
    };

    auto get_last_satisfied = [](const std::vector<Program*>& term_list, PointExample* example) -> int {
        int r = 0;
        for (int i = 0; i < term_list.size(); ++i) {
            if (term_list[i]->run(example->first) == example->second) r = i;
        }
        return r;
    };

    auto check_conditions = [](const std::vector<Program*>& condition_list, PointExample* example) -> bool {
        for (auto* condition: condition_list) {
            if (condition->run(example->first).getBool()) return true;
        }
        return false;
    };

    auto verify_condition = [=](const std::vector<Program*>& term_list, const std::vector<Program*>& condition_list, Program* condition, int id) -> int{
        for (int ind = 0; ind < example_space.size(); ++ind) {
            auto* example = example_space[ind];
            if (check_conditions(condition_list, example)) continue;
            if (condition->run(example->first).getBool()) {
                if (term_list[id]->run(example->first) != example->second) {
                    return ind;
                }
            } else {
                if (get_last_satisfied(term_list, example) == id) return ind;
            }
        }
        return -1;
    };

    used_examples.push_back(example_space[0]);

    while (true) {
        global::KSearchTimeLimit = 60;
        gettimeofday(&global::start_time, NULL);
        term_list = term_solver->getTerms(used_examples);
        if (term_list.empty()) {
            int pre = used_examples.size();
            for (int i = 0; i < 2; ++i) {
                used_examples.push_back(example_space[pre + i]);
            }
            continue;
        }
        int counter_id = verify_programs(term_list);
        //for (auto* example: used_examples) {
        //    std::cout << *example << std::endl;
        //}
        /*for (auto* term: term_list) {
            LOG(INFO) << term->toString() << std::endl;
        }*/
        if (counter_id == -1) break;
        assert(counter_id >= used_examples.size());
        for (int i = used_examples.size(); i <= counter_id; ++i) {
            used_examples.push_back(example_space[i]);
        }
    }
    for (auto* term: term_list) {
        LOG(INFO) << term->toString() << std::endl;
    }
    // Solve Condition
    std::vector<Program*> condition_list;
    int param_num = example_space[0]->first.size();
    std::vector<int> param_list;
    for (int i = 0; i < param_num; ++i) param_list.push_back(i);
    unifier->param_list = param_list;
    unifier->extra_list = term_list;

    for (int i = 0; i + 1 < term_list.size(); ++i) {
        auto* term = term_list[i];
        while (1) {
            std::vector<PointExample *> positive_list, negative_list;
            for (auto *example: used_examples) {
                if (check_conditions(condition_list, example)) continue;
                if (term->run(example->first) != example->second) negative_list.push_back(example);
                else if (get_last_satisfied(term_list, example) == i) positive_list.push_back(example);
            }
            auto* condition = unifier->getCondition(positive_list, negative_list);
            auto counter_id = verify_condition(term_list, condition_list, condition, i);
            if (counter_id == -1) {
                condition_list.push_back(condition);
                break;
            }
            assert(counter_id >= used_examples.size());
            for (int j = used_examples.size(); j <= counter_id; ++j) {
                used_examples.push_back(example_space[j]);
            }
        }
    }

    return {used_examples.size(), mergeIte(term_list, condition_list)};
}

Program* CLIASolver::synthesize2(const std::vector<PointExample*>& example) {
    auto* start = new Program({}, new ConstSemantics(0));
    std::vector<PointExample*> example_space;

    auto verify = [=](Program* p) -> PointExample *{
        for (auto* e: example) {
            if (p->run(e->first) != e->second) {
                return e;
            }
        }
        return nullptr;
    };

    auto* counter_example = verify(start);
    if (!counter_example) return start;
    example_space.push_back(counter_example);
    std::vector<Program*> term_list, condition_list;

    std::vector<int> param_list;
    for (int i = 0; i < example[0]->first.size(); ++i) param_list.push_back(i);
    unifier->param_list = param_list;
    unifier->extra_list = term_list;

    while (true) {
        auto* last_example = example_space[example_space.size() - 1];
        bool is_occur = false;
        for (auto* term: term_list) {
            if (term->run(last_example->first) == last_example->second) {
                is_occur = true; break;
            }
        }
        if (!is_occur) {
            auto new_term = term_solver->getTerms(example_space);
            // for (auto* example: example_space) std::cout << *example << std::endl;
            LOG(INFO) << "New term";
            for (auto* term: new_term) LOG(INFO) << term->toString() << std::endl;
            unifier->extra_list = new_term;

            std::vector<Program*> new_condition;
            for (int id = 0; id + 1 < new_term.size(); ++id) {
                auto* term = new_term[id];
                Program* cond = nullptr;
                for (int i = 0; i + 1 < term_list.size(); ++i) {
                    if (term->toString() == term_list[i]->toString()) {
                        cond = condition_list[i];
                        break;
                    }
                }
                if (!cond) {
                    cond = new Program({}, new ConstSemantics(new BoolValue(true)));
                }
                new_condition.push_back(cond);
            }
            term_list = new_term;
            condition_list = new_condition;
        }
        std::vector<PointExample*> rem_example = example_space;
        for (int i = 0; i + 1 < term_list.size(); ++i) {
            std::vector<PointExample*> positive_list, negative_list, mid_list;
            auto* current_term = term_list[i];
            for (auto* current_example: rem_example) {
                if (current_term->run(current_example->first) == current_example->second) {
                    bool is_mid = false;
                    for (int j = i + 1; j < term_list.size(); ++j) {
                        if (term_list[j]->run(current_example->first) == current_example->second) {
                            is_mid = true;
                            break;
                        }
                    }
                    if (is_mid) {
                        mid_list.push_back(current_example);
                    } else positive_list.push_back(current_example);
                } else negative_list.push_back(current_example);
            }

            auto* condition = condition_list[i];
            bool is_valid = true;
            for (auto* example: positive_list) {
                if (!condition->run(example->first).getBool()) {
                    is_valid = false; break;
                }
            }
            if (is_valid) {
                for (auto* example: negative_list) {
                    if (condition->run(example->first).getBool()) {
                        is_valid = false; break;
                    }
                }
            }

            if (!is_valid) {
                // std::cout << "solve for " << current_term->toString() << std::endl;
                condition = unifier->getCondition(positive_list, negative_list);
            }
            condition_list[i] = condition;
            rem_example = negative_list;
            for (auto* example: mid_list) {
                if (!(condition->run(example->first).getBool())) rem_example.push_back(example);
            }
        }

        auto* current_program = mergeIte(term_list, condition_list);
        LOG(INFO) << "Turn #" << example_space.size() << ": " << current_program->toString() << std::endl;
#ifdef DEBUG
        for (auto* example: example_space) assert(current_program->run(example->first) == example->second);
#endif
        recorder::recorder.pushStage("verify");
        counter_example = verify(current_program);
        recorder::recorder.popStage();
        if (counter_example) {
            example_space.push_back(counter_example);
        } else {
            return current_program;
        }
    }
}

std::pair<int, Program*> CLIASolver::cegis_for_decision_tree(Specification *spec) {
    
    LOG(INFO) << "decision tree unifier (cegis)"  << std::endl;
    auto* start = new Program({}, new ConstSemantics(0));
    
    std::vector<PointExample*> example_space;
    auto* counter_example = spec->verify(start, true);
    if (!counter_example) return {0, start};
    example_space.push_back(counter_example);
    std::vector<Program*> term_list, condition_list;

    std::vector<int> param_list;
    for (int i = 0; i < spec->param_list.size(); ++i) param_list.push_back(i);
    unifier->param_list = param_list;
    unifier->extra_list = term_list;

    int pred_num = 10;    
    
    while (true) {
        auto* last_example = example_space[example_space.size() - 1];
        bool is_occur = false;
        for (auto* term: term_list) {
            if (term->run(last_example->first) == last_example->second) {
                is_occur = true; break;
            }
        }
        if (!is_occur) {
            auto new_term = term_solver->getTerms(example_space);
            // for (auto* example: example_space) std::cout << *example << std::endl;
            LOG(INFO) << "New term";
            for (auto* term: new_term) LOG(INFO) << term->toString() << std::endl;
            unifier->extra_list = new_term;

            std::vector<Program*> new_condition;
            for (int id = 0; id + 1 < new_term.size(); ++id) {
                auto* term = new_term[id];
                Program* cond = nullptr;
                for (int i = 0; i + 1 < term_list.size(); ++i) {
                    if (term->toString() == term_list[i]->toString()) {
                        cond = condition_list[i];
                        break;
                    }
                }
                if (!cond) {
                    cond = new Program({}, new ConstSemantics(new BoolValue(true)));
                }
                new_condition.push_back(cond);
            }
            term_list = new_term;
            condition_list = new_condition;
        }
        auto* cmp = new NonTerminal("cmp", TBOOL);
        auto* atom = new NonTerminal("param", TINT);
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
        cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("<="), {expr, expr}));
        cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("<"), {expr, expr}));
        cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("and"), {cmp, cmp}));
        cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("or"), {cmp, cmp}));
        auto* grammar = new Grammar(cmp, {cmp, atom, expr});
        auto* enumerator = new Enumerator(grammar, 1000);
        auto pred_list = enumerator->enumerateProgram(pred_num);
        pred_num ++;
        
        auto* current_program = unifier->unify(example_space, term_list, pred_list);
        if (current_program == nullptr) continue;

        LOG(INFO) << "Turn #" << example_space.size() << ": " << current_program->toString() << std::endl;
#ifdef DEBUG
        for (auto* example: example_space) assert(current_program->run(example->first) == example->second);
#endif
        recorder::recorder.pushStage("verify");
        counter_example = spec->verify(current_program, true);
        recorder::recorder.popStage();
        if (counter_example) {
            example_space.push_back(counter_example);
        } else {
            return {int(example_space.size()), current_program};
        }
    }

    
}

std::pair<int, Program*> CLIASolver::cegis(Specification *spec) {
    if (this->unifier->getType() == "DecisionTree"){
        return this->cegis_for_decision_tree(spec);
    }

    LOG(INFO) << "optimized unifier (cegis)"  << std::endl;
    auto* start = new Program({}, new ConstSemantics(0));
    std::vector<PointExample*> example_space;
    auto* counter_example = spec->verify(start, true);
    if (!counter_example) return {0, start};
    example_space.push_back(counter_example);
    std::vector<Program*> term_list, condition_list;

    std::vector<int> param_list;
    for (int i = 0; i < spec->param_list.size(); ++i) param_list.push_back(i);
    unifier->param_list = param_list;
    unifier->extra_list = term_list;

    while (true) {
        auto* last_example = example_space[example_space.size() - 1];
        bool is_occur = false;
        for (auto* term: term_list) {
            if (term->run(last_example->first) == last_example->second) {
                is_occur = true; break;
            }
        }
        if (!is_occur) {
            auto new_term = term_solver->getTerms(example_space);
            // for (auto* example: example_space) std::cout << *example << std::endl;
            LOG(INFO) << "New term";
            for (auto* term: new_term) LOG(INFO) << term->toString() << std::endl;
            unifier->extra_list = new_term;

            std::vector<Program*> new_condition;
            for (int id = 0; id + 1 < new_term.size(); ++id) {
                auto* term = new_term[id];
                Program* cond = nullptr;
                for (int i = 0; i + 1 < term_list.size(); ++i) {
                    if (term->toString() == term_list[i]->toString()) {
                        cond = condition_list[i];
                        break;
                    }
                }
                if (!cond) {
                    cond = new Program({}, new ConstSemantics(new BoolValue(true)));
                }
                new_condition.push_back(cond);
            }
            term_list = new_term;
            condition_list = new_condition;
        }
        std::vector<PointExample*> rem_example = example_space;
        for (int i = 0; i + 1 < term_list.size(); ++i) {
            std::vector<PointExample*> positive_list, negative_list, mid_list;
            auto* current_term = term_list[i];
            for (auto* current_example: rem_example) {
                if (current_term->run(current_example->first) == current_example->second) {
                    bool is_mid = false;
                    for (int j = i + 1; j < term_list.size(); ++j) {
                        if (term_list[j]->run(current_example->first) == current_example->second) {
                            is_mid = true;
                            break;
                        }
                    }
                    if (is_mid) {
                        mid_list.push_back(current_example);
                    } else positive_list.push_back(current_example);
                } else negative_list.push_back(current_example);
            }

            auto* condition = condition_list[i];
            bool is_valid = true;
            for (auto* example: positive_list) {
                if (!condition->run(example->first).getBool()) {
                    is_valid = false; break;
                }
            }
            if (is_valid) {
                for (auto* example: negative_list) {
                    if (condition->run(example->first).getBool()) {
                        is_valid = false; break;
                    }
                }
            }

            if (!is_valid) {
                // std::cout << "solve for " << current_term->toString() << std::endl;
                condition = unifier->getCondition(positive_list, negative_list);
            }
            condition_list[i] = condition;
            rem_example = negative_list;
            for (auto* example: mid_list) {
                if (!(condition->run(example->first).getBool())) rem_example.push_back(example);
            }
        }

        auto* current_program = mergeIte(term_list, condition_list);
        LOG(INFO) << "Turn #" << example_space.size() << ": " << current_program->toString() << std::endl;
#ifdef DEBUG
        for (auto* example: example_space) assert(current_program->run(example->first) == example->second);
#endif
        recorder::recorder.pushStage("verify");
        counter_example = spec->verify(current_program, true);
        recorder::recorder.popStage();
        if (counter_example) {
            example_space.push_back(counter_example);
        } else {
            return {int(example_space.size()), current_program};
        }
    }
}

Program* CLIASolver::synthesize_for_decision_tree(const std::vector<PointExample*>& all_exps) {
    
    LOG(INFO) << "decision tree unifier (random)"  << std::endl;
    std::vector<Program*> term_list = term_solver->getTerms(all_exps);
    auto* start = new Program({}, new ConstSemantics(0));
    std::vector<PointExample*> example_space;
    auto* counter_example = verify_by_example(all_exps, start);
    if (!counter_example) return start;
    example_space.push_back(counter_example);
    std::vector<Program*>  condition_list;
    std::vector<int> param_list;
    for (int i = 0; i < counter_example->first.size() ; ++i) param_list.push_back(i);
    unifier->param_list = param_list;
    unifier->extra_list = term_list;
    int pred_num = 10;    
    while (true) {
        auto* last_example = example_space[example_space.size() - 1];
        bool is_occur = false;
        for (auto* term: term_list) {
            if (term->run(last_example->first) == last_example->second) {
                is_occur = true; break;
            }
        }
        if (!is_occur) {
            auto new_term = term_solver->getTerms(example_space);
            // for (auto* example: example_space) std::cout << *example << std::endl;
            LOG(INFO) << "New term";
            for (auto* term: new_term) LOG(INFO) << term->toString() << std::endl;
            unifier->extra_list = new_term;

            std::vector<Program*> new_condition;
            for (int id = 0; id + 1 < new_term.size(); ++id) {
                auto* term = new_term[id];
                Program* cond = nullptr;
                for (int i = 0; i + 1 < term_list.size(); ++i) {
                    if (term->toString() == term_list[i]->toString()) {
                        cond = condition_list[i];
                        break;
                    }
                }
                if (!cond) {
                    cond = new Program({}, new ConstSemantics(new BoolValue(true)));
                }
                new_condition.push_back(cond);
            }
            term_list = new_term;
            condition_list = new_condition;
        }
        
        auto* cmp = new NonTerminal("cmp", TBOOL);
        auto* atom = new NonTerminal("param", TINT);
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
        cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("<="), {expr, expr}));
        cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("<"), {expr, expr}));
        cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("and"), {cmp, cmp}));
        cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("or"), {cmp, cmp}));
        auto* grammar = new Grammar(cmp, {cmp, atom, expr});
        auto* enumerator = new Enumerator(grammar, 1000);
        auto pred_list = enumerator->enumerateProgram(pred_num);
        pred_num ++;
        
        auto* current_program = unifier->unify(example_space, term_list, pred_list);
        if (current_program == nullptr) continue;

        LOG(INFO) << "Turn #" << example_space.size() << ": " << current_program->toString() << std::endl;
#ifdef DEBUG
        for (auto* example: example_space) assert(current_program->run(example->first) == example->second);
#endif
        recorder::recorder.pushStage("verify");
        counter_example =  verify_by_example(all_exps, current_program);
        recorder::recorder.popStage();
        if (counter_example) {
            example_space.push_back(counter_example);
        } else {
            return current_program;
        }
    }
    
}

Program* CLIASolver::synthesize(const std::vector<PointExample*>& example_space) {
    if (this->unifier->getType() == "DecisionTree"){
        return this->synthesize_for_decision_tree(example_space);
    }
    LOG(INFO) << "optimized unifier (random)"  << std::endl;
    if (example_space.empty()) {
        return new Program({}, new ConstSemantics(0));
    }
    std::vector<Program*> term_list = term_solver->getTerms(example_space);
    LOG(INFO) << "term finished" << std::endl;
    for (auto* term: term_list) {
        LOG(INFO) << "term: " << term->toString() << std::endl;
    }

    std::vector<int> param_list;
    for (int i = 0; i < example_space[0]->first.size(); ++i) param_list.push_back(i);
    unifier->param_list = param_list;
    unifier->extra_list = term_list;

    std::vector<Program*> condition_list;
    std::vector<PointExample*> rem_example = example_space;
    for (int i = 0; i + 1 < term_list.size(); ++i) {
        std::vector<PointExample*> positive_list, negative_list, mid_list;
        auto* current_term = term_list[i];
        for (auto* current_example: rem_example) {
            if (current_term->run(current_example->first) == current_example->second) {
                bool is_mid = false;
                for (int j = i + 1; j < term_list.size(); ++j) {
                    if (term_list[j]->run(current_example->first) == current_example->second) {
                        is_mid = true;
                        break;
                    }
                }
                if (is_mid) {
                    mid_list.push_back(current_example);
                } else positive_list.push_back(current_example);
            } else negative_list.push_back(current_example);
        }
        Program* condition = unifier->getCondition(positive_list, negative_list);
        condition_list.push_back(condition);
        rem_example = negative_list;
        for (auto* example: mid_list) {
            if (!(condition->run(example->first).getBool())) rem_example.push_back(example);
        }
    }
    return mergeIte(term_list, condition_list);
}