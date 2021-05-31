#ifndef CPP_CLIA_SOLVER_H
#define CPP_CLIA_SOLVER_H


#include "config.h"
#include "json/json.h"
#include "recorder.h"
#include "util.h"
#include "semantics_factory.h"
#include "glog/logging.h"
#include <cstdio>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include "program.h"

#include "ilp_term_solver.h"
#include "enumerate_term_solver.h"
#include "term_solver.h"
#include "unifier.h"
#include "specification.h"
#include "optimized_unifier.h"
#include "decision_tree_unifier.h"
#include "optimized_ilp_term_solver.h"

class PBESolver {
public:
    virtual Program* synthesis(const std::vector<PointExample*>& example_list) = 0;
};

class CLIASolver {
    TermSolver* term_solver;
    Unifier* unifier;
public:
    CLIASolver(TermSolver* term_solver, Unifier* unifier);
    Program* synthesize(const std::vector<PointExample*>& example_space);
    Program* synthesize_for_decision_tree(const std::vector<PointExample*>& example_space);
    Program* synthesize2(const std::vector<PointExample*>& example_space);
    std::pair<int, Program*> dealWithRandom(const std::vector<PointExample*>& example_space);
    std::pair<int, Program*> cegis(Specification* spec);
    std::pair<int, Program*> cegis_for_decision_tree(Specification* spec);
    void clearCache() {term_solver->clearCache();}
};

class CLIASolverBuilder {
public:
    TermSolver* term_solver;
    Unifier* unifier;
    explicit CLIASolverBuilder(const std::string& term_solver_name = "optimized", const std::string& unifier_name = "optimized") {
        //optimized , optimized for PolyGen
        //enumerate , optimized for ablation_T
        //optimized , heuristic for ablation_U
        if (term_solver_name == "optimized") {
            term_solver = new OptimizedTermSolver();
        } else if (term_solver_name == "enumerate") {
            term_solver = new EnumerateTermSolver();
        } else {
            assert(0);
        }
        if (unifier_name == "optimized") {
            unifier = new OptimizedUnifier();
        } else if (unifier_name == "heuristic") {
            unifier = new DecisionTreeUnifier();
        } else{
            assert(0);
        }
    }   
    CLIASolver* build() {
        return new CLIASolver(term_solver, unifier);
    }
};


#endif //CPP_CLIA_SOLVER_H
