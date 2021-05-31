#ifndef CPP_ILP_TERM_SOLVER_H
#define CPP_ILP_TERM_SOLVER_H

#include "term_solver.h"
#include "gurobi_c++.h"

class TermConfig {
public:
    std::vector<int> assignment;
    int cover_num, size;
    std::vector<PointExample*> example_list;
    int operator < (const TermConfig& k1) {
        return cover_num < k1.cover_num;
    }
    TermConfig(std::vector<int>&& _assignment, int _cover_num, std::vector<PointExample*>&& _example_list):
        assignment(_assignment), cover_num(_cover_num), size(0), example_list(std::move(_example_list)) {
        for (int v: assignment) size += abs(v);
    }
};

namespace solver {
    int getOptimalAssignment(GRBEnv& env, const std::vector<PointExample*>& example_list, std::vector<int>& result, const std::vector<int>& initial_solution = {});
}

class BasicILPTermSolver: public TermSolver {
public:
    GRBEnv env;
    BasicILPTermSolver(): env(GRBEnv(true)) {
        env.set("LogFile", "gurobi.log");
        env.start();
    }
    virtual std::vector<Program*> getTerms(const std::vector<PointExample*>& example_list) = 0;
};

class ILPTermSolver: public BasicILPTermSolver {
    std::vector<TermConfig*> getPossibleTerm(const std::vector<PointExample*>& example_list, int rem_size);
    int getConfidence(const std::vector<int>& assignment, const std::vector<PointExample*>& example_list);
    int optimal_size = 1000000000;
    std::vector<std::vector<int> > optimal_solution;
    std::vector<Program*> atom_list;
    void searchForOptimal(const std::vector<PointExample*>& example_list, int rem, int size_sum, std::vector<std::vector<int> >& solution);
public:
    ILPTermSolver() = default;
    virtual std::vector<Program*> getTerms(const std::vector<PointExample *>& example_list);
};


#endif //CPP_ILP_TERM_SOLVER_H
