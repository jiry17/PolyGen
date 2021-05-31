#ifndef L2S_SEMANTICS_H
#define L2S_SEMANTICS_H

#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
#include "data.h"

typedef std::vector<DataList> WitnessTerm;
typedef std::vector<WitnessTerm> WitnessList;

// An abstracted class representing the global info which is possibly used by wintess functions.
class GlobalInfo {
public:
    virtual std::string getName() {return "GlobalInfo";}
};

// For a given example, an object of "ParamInfo" contains the values of all variables.
class ParamInfo: public GlobalInfo {
protected:
    const DataList *param_value;
public:
    virtual std::string getName() {return "ParamInfo";}
    ParamInfo(const DataList& _param_value): param_value(&_param_value) {}
    ParamInfo() = default;
    Data operator [] (int k) {return (*param_value)[k];}
    int size() {return param_value->size();}
};

// An abstracted class representing the semantics of an operator.
// If a new kind of operator is used, it must be implemented as a subclass of "Semantics", including its
// witness function and its semantics. Besides, this operator should also be register in "semantics_factory.h".
class Semantics {
protected:
    void check(const DataList& inp_list) {
        assert(inp_list.size() == inp_type_list.size());
        for (int i = 0; i < inp_list.size(); ++i) {
            assert(inp_list[i].getType() == inp_type_list[i]);
        }
    }
public:
    std::vector<Type> inp_type_list;
    Type oup_type;
    std::string name;
    Semantics(const std::vector<Type>& _inp_type_list, Type _oup_type, std::string _name):
        inp_type_list(_inp_type_list), oup_type(_oup_type), name(_name) {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) = 0;
    virtual ~Semantics() = default;
};

class Z3Semantics: public Semantics {
public:
    Z3Semantics(const std::vector<Type>& _inp_type_list, Type _oup_type, std::string _name):
        Semantics(_inp_type_list, _oup_type, _name) {
    }
    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx, const std::vector<z3::expr>& params) = 0;
};

// A special semantics for all constant values.
class ConstSemantics: public Z3Semantics{
public:
    Data value;
    ConstSemantics(Data _value):
        value(_value), Z3Semantics({}, _value.getType(), _value.toString()) {}
    virtual Data run(DataList &&inp, GlobalInfo *global_info) {
        return value;
    }
    z3::expr getZ3Expr(std::vector<z3::expr> &&inp, z3::context& ctx, const std::vector<z3::expr> &params) {
        return value.toZ3(ctx);
    }
};

// A special semantics for all parameters.
class ParamSemantics: public Z3Semantics {
    int id;
    Type type;
public:
    ParamSemantics(int _id, Type _type): id(_id), type(_type),
        Z3Semantics({}, _type, "Param" + std::to_string(_id)) {}
    virtual Data run(DataList &&inp, GlobalInfo *global_info) {
        ParamInfo* param_info = dynamic_cast<ParamInfo*>(global_info);
#ifdef DEBUG
        assert(param_info != nullptr);
#endif
        return (*param_info)[id];
    }

    z3::expr getZ3Expr(std::vector<z3::expr> &&inp, z3::context& ctx, const std::vector<z3::expr> &params) {
        return params[id];
    }

    int getId() {return id;}
    Type getType() {return type;}
};

class SemanticsValue: public Value {
public:
    Semantics* semantics;
    bool is_anonymous;
    SemanticsValue(Semantics* _semantics, bool _is_anonymous):
        semantics(_semantics), Value(TSEMANTICS), is_anonymous(_is_anonymous) {}
    virtual Value* copy() const {return new SemanticsValue(semantics, is_anonymous);}
    void releaseSemantics() {
        if (is_anonymous) delete semantics;
    };
    virtual ~SemanticsValue() = default;
    virtual std::string toString() const {return semantics->name;}
};

typedef std::function<Data(DataList&&, GlobalInfo* global_info)> SemanticsFunction;

class AnonymousSemantics: public Semantics {
public:
    SemanticsFunction f;
    AnonymousSemantics(SemanticsFunction _f, const std::vector<Type> inp_list, Type oup, std::string _name):
        Semantics(inp_list, oup, _name), f(_f) {
        //std::cout << "new anonymous " << std::endl;
    }

    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
        return f(std::move(inp), global_info);
    }

    virtual ~AnonymousSemantics() {
        //std::cout << "delete anonymous " << std::endl;
    }
};

namespace semantics {
    Data curry(Semantics* semantics, DataList& inp);
}
#endif //L2S_SEMANTICS_H
