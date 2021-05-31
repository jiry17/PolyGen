#include "enumerator.h"
#include "util.h"
#include <unordered_map>

namespace {

    void _splitSize(int pos, int rem, const std::vector<std::vector<int>>& pool, std::vector<int>& scheme, std::vector<std::vector<int>>& result) {
        if (pos == pool.size()) {
            if (rem == 0) {
                result.push_back(scheme);
            }
            return;
        }
        for (int value: pool[pos]) {
            if (value <= rem) {
                scheme.push_back(value);
                _splitSize(pos + 1, rem - value, pool, scheme, result);
                scheme.pop_back();
            }
        }
    }

    std::vector<std::vector<int>> splitSize(int tot, const std::vector<std::vector<int>>& pool) {
        std::vector<std::vector<int>> result;
        std::vector<int> scheme;
        _splitSize(0, tot, pool, scheme, result);
        return result;
    }

    void _mergeProgram(int pos, Rule* rule, std::vector<ProgramList*>& candidate, ProgramList& sub_list, ProgramList& result) {
        if (pos == candidate.size()) {
            result.push_back(new Program(sub_list, rule->semantics));
            return;
        }
        for (auto* program: *candidate[pos]) {
            sub_list.push_back(program);
            _mergeProgram(pos + 1, rule, candidate, sub_list, result);
            sub_list.pop_back();
        }
    }

    ProgramList mergeProgram(Rule* rule, std::vector<ProgramList*>& candidate) {
        ProgramList result, sub_program;
        _mergeProgram(0, rule, candidate, sub_program, result);
        return result;
    }
    std::vector<PointExample *> remove_covered(const std::vector<PointExample *> & example_list, Program* prog){
        std::vector<PointExample *> res;
        for(auto i : example_list){
            if(prog->run(i->first) != i->second) res.push_back(i);
        }
        return res;
    }
}

std::vector<Program *> Enumerator::enumerateProgram(int num_limit) {
    std::vector<Program*> result;
    std::vector<ProgramStorage> storage_list;
    grammar->indexSymbols();
    for (auto* symbol: grammar->symbol_list) {
        storage_list.push_back({{}});
    }
    for (int size = 1; size <= size_upper_bound ; ++size) {
        for (auto* symbol: grammar->symbol_list) {
            storage_list[symbol->id].emplace_back();
            for (auto* rule: symbol->rule_list) {
                std::vector<std::vector<int>> pool;
                for (auto* param_symbol: rule->param_list) {
                    int param_id = param_symbol->id;
                    std::vector<int> choice;
                    for (int i = 1; i < size; ++i) {
                        if (!storage_list[param_id][i].empty()) {
                            choice.push_back(i);
                        }
                    }
                    pool.push_back(choice);
                }
                auto split_list = splitSize(size - 1, pool);
                for (auto& split: split_list) {
                    std::vector<ProgramList*> candidate;
                    for (int i = 0; i < split.size(); ++i) {
                        candidate.push_back(&storage_list[rule->param_list[i]->id][split[i]]);
                    }
                    for (auto* program: mergeProgram(rule, candidate)) {
                        storage_list[symbol->id][size].push_back(program);
                        if (symbol->name == grammar->start_symbol->name && v->verify(program)) {
                            result.push_back(program);
                            if (result.size() >= num_limit) {
                                return result;
                            }
                        }
                    }

                }
            }
        }
    }
    return result;
}


std::vector<Program *> Enumerator::enumerateProgram(const std::vector<PointExample *>&example_list) {
    std::vector<Program*> result;
    std::vector<ProgramStorage> storage_list;
    std::vector<PointExample *> cur_example_list = example_list;
    grammar->indexSymbols();
    for (auto* symbol: grammar->symbol_list) {
        storage_list.push_back({{}});
    }
    for (int size = 1; size <= size_upper_bound ; ++size) {
        for (auto* symbol: grammar->symbol_list) {
            storage_list[symbol->id].emplace_back();
            for (auto* rule: symbol->rule_list) {
                std::vector<std::vector<int>> pool;
                for (auto* param_symbol: rule->param_list) {
                    int param_id = param_symbol->id;
                    std::vector<int> choice;
                    for (int i = 1; i < size; ++i) {
                        if (!storage_list[param_id][i].empty()) {
                            choice.push_back(i);
                        }
                    }
                    pool.push_back(choice);
                }
                auto split_list = splitSize(size - 1, pool);
                for (auto& split: split_list) {
                    std::vector<ProgramList*> candidate;
                    for (int i = 0; i < split.size(); ++i) {
                        candidate.push_back(&storage_list[rule->param_list[i]->id][split[i]]);
                    }
                    for (auto* program: mergeProgram(rule, candidate)) {
                        storage_list[symbol->id][size].push_back(program);
                        if (symbol->name == grammar->start_symbol->name && v->verify(program)) {
                            result.push_back(program);
                            
                            //if (result.size() >= num_limit) return result;
                            
                            
                            cur_example_list = remove_covered(cur_example_list, result.back());
                            if(cur_example_list.empty()) return result;
                        }
                    }

                }
            }
        }
    }
    return result;
}
