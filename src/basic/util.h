//
// Created by pro on 2020/12/7.
//

#ifndef CPP_UTIL_H
#define CPP_UTIL_H

#include "data.h"
#include "json/json.h"
#include <unistd.h>

namespace util {
    extern std::string dataList2String(const DataList& data);
    extern std::string loadStringFromFile(const std::string file_name);
    extern Type string2Type(const std::string name);
    extern Data parseDataFromJson(Json::Value node);
    extern Json::Value loadJsonFromFile(std::string file_name);
    extern std::string type2String(Type type);
    extern z3::expr getVariable(Type type, z3::context& ctx, std::string name = "");
    extern Data getVariableData(Type type, z3::expr& expr, const z3::model& model);
    extern int getRandomInt();
    extern double getTimePeriod(const timeval& start, const timeval& end);
    extern bool checkTimeOut();
}


#endif //CPP_UTIL_H
