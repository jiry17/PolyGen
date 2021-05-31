//
// Created by pro on 2021/2/5.
//

#include "parser.h"

#include "util.h"
#include "config.h"

Specification * parser::parseSpecification(const std::string& file_name) {
    std::string json_file = std::to_string(rand()) + ".json";
    std::string parse_command = "python3 " + config::KParserPath + " " + file_name + " " + json_file;
    std::system(parse_command.c_str());
    Json::Value root = util::loadJsonFromFile(json_file);
    std::string rm_command = "rm " + json_file;
    std::system(rm_command.c_str());
    return new Specification(root);
}

std::vector<PointExample*> parser::parseExampleFile(const std::string& file_name) {
    FILE* file = fopen(file_name.c_str(), "r");
    int n, m;
    fscanf(file, "%d", &n);
    for (; n; --n) {
        int k;
        fscanf(file, "%d", &k);
        global::int_consts.push_back(k);
    }
    fscanf(file, "%d%d", &n, &m);
    if (n <= 5) config::KTermIntMax = 10, config::KOrLimit = 4;
    std::vector<PointExample*> example_space;
    for (; m; --m) {
        DataList inp;
        for (int i = 1; i <= n; ++i) {
            int k;
            fscanf(file, "%d", &k);
            inp.emplace_back(k);
        }
        int oup;
        fscanf(file, "%d", &oup);
        example_space.push_back(new PointExample(inp, oup));
    }
    return example_space;
}