#ifndef L2S_PROGRAM_H
#define L2S_PROGRAM_H

#include "semantics.h"

// An AST of a program (possibly partial)
// "semantics = nullptr" represents a placeholder in a partial AST.
class Program {
    Data run(GlobalInfo* inp);
public:
    std::vector<Program*> sub_list;
    Semantics* semantics;

    Program(const std::vector<Program*> _sub_list, Semantics* _semantics): sub_list(_sub_list), semantics(_semantics) {}
    Program(Program* program) {
        semantics = program->semantics;
        for (auto sub_program: program->sub_list) {
            sub_list.push_back(new Program(sub_program));
        }
    }
    ~Program() {
        for (auto sub_program: sub_list) {
            delete sub_program;
        }
    }
    Program(): semantics(nullptr) {}
    Program* copy() {
        std::vector<Program*> cloned_sub_list;
        for (auto* sub_program: sub_list) {
            cloned_sub_list.push_back(sub_program->copy());
        }
        return new Program(cloned_sub_list, semantics);
    }
    z3::expr getZ3Expr(z3::context& ctx, const std::vector<z3::expr>& param_list);
    std::string toString();
    void print();
    Data run(const DataList& inp);
    int size();
};


#endif //L2S_PROGRAM_H
