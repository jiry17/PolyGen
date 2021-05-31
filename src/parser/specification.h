//
// Created by pro on 2021/2/5.
//

#ifndef CPP_SPECIFICATION_H
#define CPP_SPECIFICATION_H

#include "data.h"
#include "program.h"
#include "json/json.h"

class ExampleGenerator {
public:

};

class Specification {
    void checkPointWise();
    z3::model getModel(z3::solver& S, bool is_random);
    PointExample* getExampleFromSolver(z3::solver& S, bool is_random);
    z3::expr buildCons(std::string name = "result");
    z3::expr buildProgramCons(const std::vector<Program*>& candidate_programs);
    z3::expr buildDefinition();
public:
    std::vector<Type> var_types;
    std::vector<Type> param_types;
    std::vector<Json::Value> param_list;
    z3::context ctx;
    std::vector<z3::expr> z3_vars, z3_params;
    Json::Value cons_root;
    Type return_type;
    Specification(Json::Value root);
    PointExample* buildExample(const DataList& inp);
    PointExample* verifyProgramList(const std::vector<Program*> &program, bool is_random);
    PointExample* verify(Program* program, bool is_random);
    PointExample* verifyConditionNegative(Program* term, Program* condition, const std::vector<Program*>& previous_conditions, bool is_random);
    PointExample* verifyConditionPositive(Program* term, const std::vector<Program*>& remain_terms, Program* condition, const std::vector<Program*>& previous_conditions, bool is_random);
    void print() const;
};


#endif //CPP_SPECIFICATION_H
