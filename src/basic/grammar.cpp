//
// Created by pro on 2020/11/23.
//

#include "grammar.h"
#include "semantics_factory.h"
#include "config.h"
#include "json/json.h"
#include "glog/logging.h"
#include <fstream>
#include <sstream>

namespace {
    Json::Value loadFromFile(std::string file_name) {
        std::ifstream inp(file_name, std::ios::out);
        std::stringstream buf;
        Json::Reader reader;
        Json::Value root;
        buf << inp.rdbuf();
        inp.close();
        std::string json_string = buf.str();
        assert(reader.parse(json_string, root));
        return root;
    }

    Type parseType(const std::string& name) {
        if (name == "list") return TLIST;
        if (name == "int") return TINT;
        if (name == "semantics") return TSEMANTICS;
        assert(0);
    }

    std::string getTypeName(Type type) {
        if (type == TINT) return "int_expr";
        else if (type == TLIST) return "list_expr";
        else assert(0);
    }
}

Grammar * grammar::getDeepCoderDSL(bool is_single) {
    auto grammar_root = loadFromFile(config::KGrammarFilePath);
    auto symbol_name_list = grammar_root.getMemberNames();
    std::map<std::string, NonTerminal*> symbol_map;
    std::vector<NonTerminal*> symbol_list;
    for (auto& name: symbol_name_list) {
        if (is_single && name == "start") continue;
        auto symbol_root = grammar_root[name];
        auto type = parseType(symbol_root["type"].asString());
        auto* symbol = new NonTerminal(name, type);
        symbol_map[name] = symbol;
        symbol_list.push_back(symbol);
    }
    for (auto& name: symbol_name_list) {
        if (is_single && name == "start") continue;
        auto symbol_root = grammar_root[name];
        auto* symbol = symbol_map[name];
        for (auto& rule: symbol_root["rule"]) {
            if (rule.isInt()) {
                int value = rule.asInt();
                symbol->rule_list.push_back(new Rule(new ConstSemantics(Data(new IntValue(value))), {}));
            } else if (rule.isString()) {
                std::string name = rule.asString();
                auto* semantics = semantics::getSemanticsFromName(name);
                symbol->rule_list.push_back(new Rule(new ConstSemantics(Data(new SemanticsValue(semantics, false))), {}));
            } else {
                assert(rule.isArray() && rule.size() > 1);
                std::string op = rule[0].asString();
                auto* semantics = semantics::getSemanticsFromName(op);
                std::vector<NonTerminal*> param_list;
                for (int i = 1; i < rule.size(); ++i) {
                    std::string param_name = rule[i].asString();
                    param_list.push_back(symbol_map[param_name]);
                }
                symbol->rule_list.push_back(new Rule(semantics, param_list));
            }
        }
    }
    auto* start_symbol = symbol_map[is_single ? "int_expr" : "start"];
    return new Grammar(start_symbol, symbol_list);
}

NonTerminal * Grammar::searchByName(std::string name) {
    for (auto* symbol: symbol_list) {
        if (symbol->name == name) {
            return symbol;
        }
    }
    assert(0);
}

void Grammar::addParam(const std::vector<Type> &type_list) {
    for (int i = 0; i < type_list.size(); ++i) {
        std::string name = getTypeName(type_list[i]);
        NonTerminal* target_symbol = searchByName(name);
        target_symbol->rule_list.push_back(new Rule(new ParamSemantics(i, type_list[i]), {}));
    }
}

void Grammar::addAuxiliarySemantics(const ExtraSemantics& semantics) {
    std::vector<NonTerminal*> param_list;
    for (auto inp_name: semantics.inp_name) {
        param_list.push_back(searchByName(inp_name));
    }
    auto* target_symbol = searchByName(semantics.oup_name);
    for (auto* rule: target_symbol->rule_list) {
        if (rule->semantics->name == semantics.semantics->name) {
            return;
        }
    }
    if (!semantics.semantics->inp_type_list.empty() && param_list.empty()) {
        target_symbol->rule_list.push_back(new Rule(new ConstSemantics(Data(new SemanticsValue(semantics.semantics, false))), param_list));
    } else {
        target_symbol->rule_list.push_back(new Rule(semantics.semantics, param_list));
    }
}

void Grammar::indexSymbols() {
    for (int i = 0; i < symbol_list.size(); ++i) {
        symbol_list[i]->id = i;
    }
}