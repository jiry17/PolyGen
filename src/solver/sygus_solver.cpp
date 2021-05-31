//
// Created by pro on 2021/2/6.
//

#include "sygus_solver.h"

Program * CEGISSolver::synthesis() {
    std::vector<PointExample*> example_list;
    while (true) {
        Program* program = solver->synthesis(example_list);
        auto* counter_example = spec->verify(program, true);
        if (!counter_example) return program;
        example_list.push_back(counter_example);
    }
}