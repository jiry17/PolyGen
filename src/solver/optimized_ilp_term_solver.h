#ifndef CPP_OPTIMIZED_ILP_TERM_SOLVER_H
#define CPP_OPTIMIZED_ILP_TERM_SOLVER_H

#include "term_solver.h"

// The term solver used in PolyGen.
class OptimizedTermSolver: public BasicILPTermSolver {
    int k_limit;
    std::vector<Program*> getTerms(const std::vector<PointExample*>& example_list, int N, int K);
    std::vector<Program*> getTerms(const std::vector<PointExample*>& example_list, int N);
public:
    OptimizedTermSolver(int _k_limit = -1): k_limit(_k_limit) {}
    virtual std::vector<Program *> getTerms(const std::vector<PointExample *> &example_list);
    virtual void clearCache();
};


#endif //CPP_OPTIMIZED_ILP_TERM_SOLVER_H
