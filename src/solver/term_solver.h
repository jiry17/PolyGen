#ifndef CPP_TERM_SOLVER_H
#define CPP_TERM_SOLVER_H


#include "program.h"
#include "data.h"

class TermSolver {
protected:
    std::vector<Program*> getTermList(int size_limit);
    int solveCLIAParam(const std::vector<PointExample> &example_list, std::vector<int> &result, bool is_require_single=false);
    int solveCLIAParam(const std::vector<PointExample*> &example_list, std::vector<int> &result, bool is_require_single=false);
    Program* merge(const std::vector<Program*>& atom_list, const std::vector<int>& param_list);
public:
    std::vector<int> param_list;
    virtual std::vector<Program*> getTerms(const std::vector<PointExample*>& example_list) = 0;
    virtual void clearCache() {}
};

class GaussTermSolver: public TermSolver {
public:
    virtual std::vector<Program*> getTerms(const std::vector<PointExample*>& example_list);
};


#endif //CPP_TERM_SOLVER_H
