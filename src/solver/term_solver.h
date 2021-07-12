#ifndef CPP_TERM_SOLVER_H
#define CPP_TERM_SOLVER_H


#include "program.h"
#include "data.h"
#include "gurobi_c++.h"

// An abstracted class for term solvers.
class TermSolver {
public:
    std::vector<int> param_list;
    virtual std::vector<Program*> getTerms(const std::vector<PointExample*>& example_list) = 0;
    virtual void clearCache() {}
};


// The client term solver on CLIA, which uses gurobi to find the simplest program satisfying several given examples.
namespace solver {
    int getOptimalAssignment(GRBEnv& env, const std::vector<PointExample*>& example_list, std::vector<int>& result);
}

// An abstracted term solver that initialize gurobi.
class BasicILPTermSolver: public TermSolver {
public:
    GRBEnv env;
    BasicILPTermSolver(): env(GRBEnv(true)) {
        env.set("LogFile", "gurobi.log");
        env.start();
    }
    virtual std::vector<Program*> getTerms(const std::vector<PointExample*>& example_list) = 0;
};


#endif //CPP_TERM_SOLVER_H
