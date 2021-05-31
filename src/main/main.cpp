#include <iostream>
#include "z3++.h"
#include "clia_solver.h"
#include "config.h"
#include "optimized_ilp_term_solver.h"

#include "glog/logging.h"
#include "gflags/gflags.h"
#include "semantics_factory.h"

#include <ctime>
#include "recorder.h"
#include "data.h"
#include "parser.h"
using namespace std;

int main(int argc, char** argv) {
    std::string inp_file, oup_file, type;
    inp_file = argv[1];
    oup_file = argv[2];
    type = argv[3];
    FILE* oup = fopen(oup_file.c_str(), "w");
    std::string term_solver = "optimized", unifier = "optimized";
    if (argc == 5){
        string ablation_typ = argv[4];
        if (ablation_typ == "1") term_solver = "enumerate"; else
        if (ablation_typ == "2") unifier = "heuristic"; 
    }
    auto *builder = new CLIASolverBuilder(term_solver, unifier);
    auto *solver = builder->build();
    if (type == "example") {
        auto example_space = parser::parseExampleFile(inp_file);
        auto *result = solver->synthesize(example_space);
        fprintf(oup, "%s\n", result->toString().c_str());
    } else if (type == "cegis") {
        auto* spec = parser::parseSpecification(inp_file);
        auto result = solver->cegis(spec);
        fprintf(oup, "%d\n", result.first);
        fprintf(oup, "%s\n", result.second->toString().c_str());
    }
    return 0;
}
