//
// Created by pro on 2021/2/5.
//

#ifndef CPP_PARSER_H
#define CPP_PARSER_H

#include "specification.h"

namespace parser{
    extern Specification* parseSpecification(const std::string& path);
    extern std::vector<PointExample*> parseExampleFile(const std::string& path);
}

#endif //CPP_PARSER_H
