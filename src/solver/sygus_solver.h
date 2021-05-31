//
// Created by pro on 2021/2/6.
//

#ifndef CPP_SYGUS_SOLVER_H
#define CPP_SYGUS_SOLVER_H

#include "specification.h"
#include "clia_solver.h"

class SyGuSSolver {
public:
    Specification* spec;
    SyGuSSolver(Specification* _spec): spec(_spec) {}
    virtual Program* synthesis() = 0;
};

class CEGISSolver: public SyGuSSolver {
public:
    PBESolver* solver;
    CEGISSolver(PBESolver* _solver, Specification* _spec): solver(_solver), SyGuSSolver(_spec) {}
    virtual Program* synthesis();
};


#endif //CPP_SYGUS_SOLVER_H
