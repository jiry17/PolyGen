#ifndef L2S_VALUE_H
#define L2S_VALUE_H

#include <cassert>
#include <vector>
#include <algorithm>

#include "config.h"

enum Type {
    TINT, TBOOL, TLIST, TNONE, TSEMANTICS
};

typedef std::vector<int> LIST;

class Value {
    Type type;
public:
    Type getType() const {return type;}
    Value(Type _type): type(_type) {}
    virtual Value* copy() const = 0;
    virtual ~Value() = default;
    virtual std::string toString() const = 0;
};

class IntValue: public Value {
    int value;
public:
    IntValue(int _value): Value(TINT), value(_value) {}
    virtual Value* copy() const {return new IntValue(value);}
    int getValue() const {return value;}
    virtual ~IntValue() = default;
    virtual std::string toString() const {return std::to_string(value);}
};

class BoolValue: public Value {
    bool value;
public:
    BoolValue(bool _value): Value(TBOOL), value(_value) {}
    virtual Value* copy() const {return new BoolValue(value);}
    bool getValue() const {return value;}
    virtual ~BoolValue() = default;
    virtual std::string toString() const {return value ? "true" : "false";}
};

class ListValue: public Value {
    LIST value;
public:
    int l_default, r_default;
    ListValue(const LIST& _value): Value(TLIST), value(_value), l_default(config::KDefaultValue), r_default(config::KDefaultValue) {}
    ListValue(const LIST& _value, int _l, int _r): Value(TLIST), value(_value), l_default(_l), r_default(_r) {}
    ListValue(LIST&& _value): value(_value), Value(TLIST), l_default(config::KDefaultValue), r_default(config::KDefaultValue) {}
    virtual Value* copy() const {return new ListValue(value, l_default, r_default);}
    LIST getValue() const {return value;}
    LIST moveValue() {return std::move(value);}
    virtual ~ListValue() = default;
    virtual std::string toString() const {
        std::string result = "{";
        for (int i = 0; i < value.size(); ++i) {
            if (i) result += ",";
            result += std::to_string(value[i]);
        }
        return result + "}";
    }
};

class NoneValue: public Value {
public:
    NoneValue(): Value(TNONE) {}
    virtual Value* copy() const {return new NoneValue();}
    virtual ~NoneValue() = default;
    virtual std::string toString() const {return "None";}
};


#endif //L2S_VALUE_H
