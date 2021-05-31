#ifndef L2S_DATA_H
#define L2S_DATA_H

#include "value.h"
#include "json/json.h"
#include "z3++.h"

#include <cassert>
#include <vector>
#include <iostream>

// A common class representing all possible values.
// If a new type of values is used, this type must be registered in this class.
class Data {
public:
    Value* value;
    Type getType() const {return value->getType();}
    Data(Value* _value): value(_value) {}

    Data(): value(nullptr) {}
    Data(int _value): value(new IntValue(_value)) {}
    Data(bool _value): value(new BoolValue(_value)) {}
    Data(const Data& _data): value(_data.value->copy()) {/*std::cout << "copy" << std::endl;*/}
    Data(Data&& data): value(data.value) {
        //std::cout << "move&&" << std::endl;
        data.value = nullptr;
    }
    Data(Json::Value node) {
        if (node["value_type"] == "Int") {
            value = new IntValue(node["value"].asInt());
        } else if (node["value_type"] == "Bool") {
            value = new BoolValue(node["value"].asBool());
        } else assert(0);
    }
    Data& operator = (Data&& data) {
        delete value;
        value = data.value;
        //std::cout << "move&&" << std::endl;
        data.value = nullptr;
        return *this;
    }
    ~Data() {
        delete value;
        value = nullptr;
    }

    Data& operator = (const Data& data) {
        delete value;
        //std::cout << "copy" << std::endl;
        value = data.value->copy();
        return *this;
    }

    std::string toString() const {
        return value->toString();
    }

    int getInt() const {
#ifdef DEBUG
        assert(value->getType() == TINT);
#endif
        return dynamic_cast<IntValue*>(value)->getValue();
    }

    bool getBool() const {
#ifdef DEBUG
        assert(value->getType() == TBOOL);
#endif
        return dynamic_cast<BoolValue*>(value)->getValue();
    }

    LIST getList() const {
#ifdef DEBUG
        assert(value->getType() == TLIST);
#endif
        return dynamic_cast<ListValue*>(value)->getValue();
    }

    int getLDefault() const {
#ifdef DEBUG
        assert(value->getType() == TLIST);
#endif
        return dynamic_cast<ListValue*>(value)->l_default;
    }

    int getRDefault() const {
#ifdef DEBUG
        assert(value->getType() == TLIST);
#endif
        return dynamic_cast<ListValue*>(value)->r_default;
    }

    LIST moveList() const {
#ifdef DEBUG
        assert(value->getType() == TLIST);
#endif
        return dynamic_cast<ListValue*>(value)->moveValue();
    }

    bool operator == (const Data& data) const {
        if (getType() != data.getType()) return false;
        switch (getType()) {
            case TINT: return getInt() == data.getInt();
            case TBOOL: return getBool() == data.getBool();
            case TLIST: return getList() == data.getList();
            case TNONE: return true;
            case TSEMANTICS: assert(0);
        }
    }

    bool operator != (const Data& data) const {
        return !((*this) == data);
    }

    z3::expr toZ3(z3::context& ctx) const {
#ifdef DEBUG
        assert(value->getType() == TINT || value->getType() == TBOOL);
#endif
        switch (getType()) {
            case TINT: return ctx.int_val(getInt());
            case TBOOL: return ctx.bool_val(getBool());
        }
    }
};


typedef std::vector<Data> DataList;

typedef std::pair<DataList, Data> PointExample;

std::ostream & operator << (std::ostream& output,Data & c);

std::ostream & operator << (std::ostream& output,DataList & c);

std::ostream & operator << (std::ostream& output, PointExample & c) ;

#endif //L2S_DATA_H
