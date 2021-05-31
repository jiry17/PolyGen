//
// Created by pro on 2020/11/23.
//

#include "semantics.h"

Data semantics::curry(Semantics *semantics, DataList &inp) {
#ifdef DEBUG
    assert(inp.size() < semantics->inp_type_list.size());
    for (int i = 0; i < inp.size(); ++i) {
        assert(inp[i].getType() == semantics->inp_type_list[i]);
    }
#endif
    if (inp.empty()) return new SemanticsValue(semantics, false);
    auto f = [semantics, inp](DataList&& remaining_inp, GlobalInfo* global_info) {
        DataList full_input;
        for (int i = 0; i < inp.size(); ++i) {
            full_input.push_back(inp[i]);
        }
        for (int i = 0; i < remaining_inp.size(); ++i) {
            full_input.push_back(remaining_inp[i]);
        }
        return semantics->run(std::move(full_input), global_info);
    };
    std::vector<Type> inp_type;
    for (int i = inp.size(); i < semantics->inp_type_list.size(); ++i) {
        inp_type.push_back(semantics->inp_type_list[i]);
    }
    std::string name = "(" + semantics->name;
    for (int i = 0; i < inp.size(); ++i) name += " " + inp[i].toString();
    auto* anonymous_semantics = new AnonymousSemantics(f, inp_type, semantics->oup_type, name + ")");
    return new SemanticsValue(anonymous_semantics, true);
}