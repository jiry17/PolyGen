#ifndef CPP_ENUMERATE_TERM_SOLVER_H
#define CPP_ENUMERATE_TERM_SOLVER_H

#include "term_solver.h"
#include "gurobi_c++.h"
#include "glog/logging.h"

// Term solver used in Eusolver, which is used only in ablation_T
class EnumerateTermSolver: public TermSolver {
public:
    int prog_num;
    virtual std::vector<Program*> getTerms(const std::vector<PointExample*>& example_list);
};



#endif //CPP_ILP_TERM_SOLVER_H
