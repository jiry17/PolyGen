//
// Created by pro on 2020/12/7.
//

#include "util.h"
#include <sys/time.h>

#include <fstream>
#include <sstream>
#include <ctime>

namespace {
    int dummy_id = 0;
}

std::string util::dataList2String(const DataList &data_list) {
    std::string result = "[";
    for (int i = 0; i < data_list.size(); ++i) {
        if (i) result += ",";
        result += data_list[i].toString();
    }
    return result + "]";
}

std::string util::loadStringFromFile(const std::string file_name) {
    std::ifstream inp(file_name, std::ios::out);
    std::stringstream buf;
    buf << inp.rdbuf();
    inp.close();
    return buf.str();
}

Type util::string2Type(const std::string name) {
    if (name == "Int") return TINT;
    if (name == "Bool") return TBOOL;
    assert(0);
}

int util::getRandomInt() {
    return std::rand() % (config::KIntMax - config::KIntMin + 1) - config::KIntMin;
}

Json::Value util::loadJsonFromFile(std::string file_name) {
    Json::Reader reader;
    Json::Value root;

    std::string json_string = util::loadStringFromFile(file_name);
    assert(reader.parse(json_string, root));
    return root;
}

std::string util::type2String(Type type) {
    if (type == TINT) return "Int";
    if (type == TBOOL) return "Bool";
    assert(0);
}

z3::expr util::getVariable(Type type, z3::context &ctx, std::string name) {
    if (name.empty()) {
        dummy_id += 1;
        name = util::type2String(type) + "@dummy_" + std::to_string(dummy_id);
    }
    if (type == TINT) return ctx.int_const(name.c_str());
    if (type == TBOOL) return ctx.bool_const(name.c_str());
    assert(0);
}

Data util::getVariableData(Type type, z3::expr& expr, const z3::model &model) {
    auto value = model.eval(expr);
    switch (type) {
        case TINT:
            if (value.is_numeral()) {
                return value.get_numeral_int();
            } else {
                return 0;
            }
        case TBOOL:
            if (value.is_bool()) {
                return new BoolValue(value.bool_value() == Z3_L_TRUE);
            } else {
                return new BoolValue(true);
            }
        default: assert(0);
    }
}

double util::getTimePeriod(const timeval &start, const timeval &end) {
    return -(start.tv_sec + start.tv_usec / 1e6) + (end.tv_sec + end.tv_usec / 1e6);
}

bool util::checkTimeOut() {
    if (global::KSearchTimeLimit == -1) return false;
    timeval end_time;
    gettimeofday(&end_time, NULL);
    return getTimePeriod(global::start_time, end_time) >= global::KSearchTimeLimit;
}