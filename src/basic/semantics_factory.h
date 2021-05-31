#ifndef CPP_SEMANTICS_FACTORY_H
#define CPP_SEMANTICS_FACTORY_H

#include "semantics.h"
#include "config.h"

class LinkSemantics: public Semantics {
public:
    LinkSemantics(): Semantics({TLIST, TLIST}, TLIST, "++") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> l_list(inp[0].moveList()), r_list(inp[1].moveList());
        for (auto v: r_list) l_list.push_back(v);
        return new ListValue(std::move(l_list));
    }
};

class HeadSemantics: public Semantics {
public:
    HeadSemantics(): Semantics({TLIST}, TINT, "head") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[0].moveList());
        if (list.empty()) return inp[0].getRDefault();
        return new IntValue(list[0]);
    }
};

class LastSemantics: public Semantics {
public:
    LastSemantics(): Semantics({TLIST}, TINT, "last") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[0].moveList());
        if (list.empty()) return inp[0].getLDefault();
        return new IntValue(list[list.size() - 1]);
    }
};

class TakeSemantics: public Semantics {
public:
    TakeSemantics(): Semantics({TINT, TLIST}, TINT, "take") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        int pos = inp[0].getInt(), n = (int)list.size();
        if (pos < 0) pos += n;
        std::vector<int> result;
        for (int i = 0; i < n && i < pos; ++i) {
            result.push_back(list[i]);
        }
        return new ListValue(std::move(result));
    }
};

class DropSemantics: public Semantics {
public:
    DropSemantics(): Semantics({TINT, TLIST}, TINT, "drop") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        int pos = inp[0].getInt(), n = (int)list.size();
        if (pos < 0) pos += n;
        pos = std::max(pos, 0);
        std::vector<int> result;
        for (int i = pos; i < n; ++i) {
            result.push_back(list[i]);
        }
        return new ListValue(std::move(result));
    }
};

class AccessSemantics: public Semantics {
public:
    AccessSemantics(): Semantics({TINT, TLIST}, TINT, "access") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        int pos = inp[0].getInt(), n = (int)list.size();
        if (pos < 0) pos += n;
        if (pos < 0) return inp[1].getLDefault();
        if (pos >= n) return inp[1].getRDefault();
        return new IntValue(list[pos]);
    }
};

class MinimumSemantics: public Semantics {
public:
    MinimumSemantics(): Semantics({TLIST}, TINT, "minimum") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[0].moveList());
        if (list.empty()) return config::KDefaultValue;
        int result = list[0];
        for (int i = 1; i < list.size(); ++i) {
            result = std::min(result, list[i]);
        }
        return new IntValue(result);
    }
};

class MaximumSemantics: public Semantics {
public:
    MaximumSemantics(): Semantics({TLIST}, TINT, "maximum") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[0].moveList());
        if (list.empty()) return -config::KDefaultValue;
        int result = list[0];
        for (int i = 1; i < list.size(); ++i) {
            result = std::max(result, list[i]);
        }
        return new IntValue(result);
    }
};

class ReverseSemantics: public Semantics {
public:
    ReverseSemantics(): Semantics({TLIST}, TLIST, "rev") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[0].moveList());
        std::reverse(list.begin(), list.end());
        return new ListValue(std::move(list));
    }
};

class SortSemantics: public Semantics {
public:
    SortSemantics(): Semantics({TLIST}, TLIST, "sort") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[0].moveList());
        std::sort(list.begin(), list.end());
        return new ListValue(std::move(list), -config::KDefaultValue, config::KDefaultValue);
    }
};

class SumSemantics: public Semantics {
public:
    SumSemantics(): Semantics({TLIST}, TINT, "sum") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[0].moveList());
        int sum = 0;
        for (int i = 0; i < list.size(); ++i) {
            sum += list[i];
        }
        return sum;
    }
};

class MapSemantics: public Semantics {
public:
    MapSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "map") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        for (int i = 0; i < list.size(); ++i) {
            list[i] = f->run({Data(new IntValue(list[i]))}, global_info).getInt();
        }
        semantics->releaseSemantics();
        return new ListValue(std::move(list));
    }
};

class FilterSemantics: public Semantics {
public:
    FilterSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "filter") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        std::vector<int> result;
        for (int i = 0; i < list.size(); ++i) {
            if (f->run({Data(new IntValue(list[i]))}, global_info).getBool()) {
                result.push_back(list[i]);
            }
        }
        semantics->releaseSemantics();
        return new ListValue(std::move(result));
    }
};

class CountSemantics: public Semantics {
public:
    CountSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "count") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        int result = 0;
        for (int i = 0; i < list.size(); ++i) {
            if (f->run({Data(new IntValue(list[i]))}, global_info).getBool()) {
                result += 1;
            }
        }
        semantics->releaseSemantics();
        return new IntValue(result);
    }
};

class ZipwithSemantics: public Semantics {
public:
    ZipwithSemantics(): Semantics({TSEMANTICS, TLIST, TLIST}, TLIST, "zipwith") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> l(inp[1].moveList()), r(inp[2].moveList());
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        std::vector<int> result;
        for (int i = 0; i < l.size() && i < r.size(); ++i) {
            result.push_back(f->run({Data(new IntValue(l[i])), Data(new IntValue(r[i]))}, global_info).getInt());
        }
        semantics->releaseSemantics();
        return new ListValue(std::move(result));
    }
};

class ScanlSemantics: public Semantics {
public:
    ScanlSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "scanl") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        if (list.empty()) return new ListValue(std::move(list));
        int pre = list[0];
        for (int i = 1; i < list.size(); ++i) {
            pre = f->run({Data(new IntValue(pre)), Data(new IntValue(list[i]))}, global_info).getInt();
            list[i] = pre;
        }
        semantics->releaseSemantics();
        return new ListValue(std::move(list));
    }
};

class ScanrSemantics: public Semantics {
public:
    ScanrSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "scanr") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        if (list.empty()) return Data(new ListValue({}));
        std::reverse(list.begin(), list.end());
        int pre = list[0];
        for (int i = 1; i < list.size(); ++i) {
            pre = f->run({Data(new IntValue(pre)), Data(new IntValue(list[i]))}, global_info).getInt();
            list[i] = pre;
        }
        std::reverse(list.begin(), list.end());
        semantics->releaseSemantics();
        return new ListValue(std::move(list));
    }
};

class PlusSemantics: public Z3Semantics {
public:
    PlusSemantics(): Z3Semantics({TINT, TINT}, TINT, "+") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt(), r = inp[1].getInt();
        int result = std::max(-config::KDefaultValue, std::min(config::KDefaultValue, l + r));
        return new IntValue(result);
    }

    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx, const std::vector<z3::expr>& params) {
        return inp[0] + inp[1];
    }
};

class MinusSemantics: public Z3Semantics {
public:
    MinusSemantics(): Z3Semantics({TINT, TINT}, TINT, "-") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt(), r = inp[1].getInt();
        int result = std::max(-config::KDefaultValue, std::min(config::KDefaultValue, l - r));
        return new IntValue(result);
    }

    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx,const std::vector<z3::expr>& params) {
        return inp[0] - inp[1];
    }
};

class TimeSemantics: public Z3Semantics {
public:
    TimeSemantics(): Z3Semantics({TINT, TINT}, TINT, "*") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt(), r = inp[1].getInt();
        long long result = l * r;
        if (result > config::KDefaultValue) result = config::KDefaultValue;
        if (result < -config::KDefaultValue) result = -config::KDefaultValue;
        return new IntValue(int(result));
    }

    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx,const std::vector<z3::expr>& params) {
        return inp[0] * inp[1];
    }
};

class MinSemantics: public Semantics {
public:
    MinSemantics(): Semantics({TINT, TINT}, TINT, "min") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt(), r = inp[1].getInt();
        return new IntValue(std::min(l, r));
    }
};

class MaxSemantics: public Semantics {
public:
    MaxSemantics(): Semantics({TINT, TINT}, TINT, "max") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt(), r = inp[1].getInt();
        return new IntValue(std::max(l, r));
    }
};

class SquareSemantics: public Semantics {
public:
    SquareSemantics(): Semantics({TINT}, TINT, "square") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt();
        return new IntValue(l * l);
    }
};

class LqSemantics: public Z3Semantics {
public:
    LqSemantics(): Z3Semantics({TINT, TINT}, TBOOL, "<") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt(), r = inp[1].getInt();
        return new BoolValue(l < r);
    }

    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx, const std::vector<z3::expr>& params) {
        return inp[0] < inp[1];
    }
};

class GqSemantics: public Z3Semantics {
public:
    GqSemantics(): Z3Semantics({TINT, TINT}, TBOOL, ">") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt(), r = inp[1].getInt();
        return new BoolValue(l > r);
    }
    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx, const std::vector<z3::expr>& params) {
        return inp[0] > inp[1];
    }
};

class EvenSemantics: public Semantics {
public:
    EvenSemantics(): Semantics({TINT}, TBOOL, "even") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt();
        return new BoolValue(l % 2 == 0);
    }
};

class OddSemantics: public Semantics {
public:
    OddSemantics(): Semantics({TINT}, TBOOL, "odd") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt();
        return Data(new BoolValue(l % 2 == 1));
    }
};

class LengthSemantics: public Semantics {
public:
    LengthSemantics(): Semantics({TLIST}, TINT, "len") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list = inp[0].getList();
        return new IntValue(list.size());
    }
};

class ApplyTwoSemantics: public Semantics {
public:
    ApplyTwoSemantics(): Semantics({TSEMANTICS, TINT, TINT}, TINT, "apply2") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        auto result = f->run({std::move(inp[1]), std::move(inp[2])}, global_info);
        semantics->releaseSemantics();
        return std::move(result);
    }
};

class LtZeroSemantics: public Semantics {
public:
    LtZeroSemantics(): Semantics({TINT}, TBOOL, "lt_zero") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int x = inp[0].getInt();
        return new BoolValue(x < 0);
    }
};

class GtZeroSemantics: public Semantics {
public:
    GtZeroSemantics(): Semantics({TINT}, TBOOL, "gt_zero") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int x = inp[0].getInt();
        return new BoolValue(x > 0 );
    }
};

class LeqSemantics: public Z3Semantics {
public:
    LeqSemantics(): Z3Semantics({TINT, TINT}, TBOOL, "<=") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int x = inp[0].getInt(), y = inp[1].getInt();
        return new BoolValue(x <= y);
    }

    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx, const std::vector<z3::expr>& params) {
        return inp[0] <= inp[1];
    }
};

class IteIntSemantics: public Z3Semantics {
public:
    IteIntSemantics(): Z3Semantics({TBOOL, TINT, TINT}, TINT, "ite") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        if (inp[0].getBool()) return std::move(inp[1]); else return std::move(inp[2]);
    }

    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx, const std::vector<z3::expr>& params) {
        return z3::ite(inp[0], inp[1], inp[2]);
    }
};

class EqSemantics: public Z3Semantics {
public:
    EqSemantics(): Z3Semantics({TINT, TINT}, TBOOL, "=") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return inp[0].getInt() == inp[1].getInt();
    }

    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx, const std::vector<z3::expr>& params) {
        return inp[0] == inp[1];
    }
};

class FilterIndexSemantics: public Semantics {
public:
    FilterIndexSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "filter_index") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> result;
        auto list(inp[1].moveList());
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        for (int i = 0; i < list.size(); ++i) {
            if (f->run({Data(new IntValue(list[i]))}, global_info).getBool()) {
                result.push_back(i);
            }
        }
        semantics->releaseSemantics();
        return new ListValue(std::move(result));
    }
};

class NotSemantics: public Z3Semantics {
public:
    NotSemantics(): Z3Semantics({TBOOL}, TBOOL, "not") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return new BoolValue(!inp[0].getBool());
    }

    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx, const std::vector<z3::expr>& params) {
        return !inp[0];
    }
};

class GeqSemantics: public Z3Semantics {
public:
    GeqSemantics(): Z3Semantics({TINT, TINT}, TBOOL, ">=") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return new BoolValue(inp[0].getInt() >= inp[1].getInt());
    }

    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx, const std::vector<z3::expr>& params) {
        return inp[0] >= inp[1];
    }
};

class ImpliesSemantics: public Z3Semantics {
public:
    ImpliesSemantics(): Z3Semantics({TBOOL, TBOOL}, TBOOL, "=>") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return new BoolValue((!inp[0].getBool()) || inp[1].getBool());
    }

    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx, const std::vector<z3::expr>& params) {
        return z3::implies(inp[0], inp[1]);
    }
};

class AndSemantics: public Z3Semantics {
public:
    AndSemantics(): Z3Semantics({TBOOL, TBOOL}, TBOOL, "and") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return new BoolValue(inp[0].getBool() & inp[1].getBool());
    }

    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx, const std::vector<z3::expr>& params) {
        return inp[0] && inp[1];
    }
};

class PowSemantics: public Semantics {
public:
    PowSemantics(): Semantics({TINT, TINT}, TINT, "pow") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int a = inp[0].getInt(), b = inp[1].getInt();
        if (a < 0 || b < 0) return new NoneValue();
        long long ans = 1;
        for (int i = 1; i <= b; ++i) {
            ans *= a;
            if (ans >= 1e9) return new NoneValue();
        }
        return int(ans);
    }
};

class OrSemantics: public Z3Semantics {
public:
    OrSemantics(): Z3Semantics({TBOOL, TBOOL}, TBOOL, "or") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return new BoolValue(inp[0].getBool() | inp[1].getBool());
    }

    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx, const std::vector<z3::expr>& params) {
        return inp[0] || inp[1];
    }
};

class NegSemantics: public Z3Semantics {
public:
    NegSemantics(): Z3Semantics({TINT}, TINT, "neg") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return -inp[0].getInt();
    }

    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx, const std::vector<z3::expr>& params) {
        return -inp[0];
    }
};

class SingleListSemantics: public Semantics {
public:
    SingleListSemantics(): Semantics({TINT}, TLIST, "[]") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return new ListValue({inp[0].getInt()});
    }
};

class ConsSemantics: public Semantics {
public:
    ConsSemantics(): Semantics({TINT, TLIST}, TLIST, "cons") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        LIST result = {inp[0].getInt()};
        for (auto& w: inp[1].moveList()) {
            result.push_back(w);
        }
        return new ListValue(std::move(result));
    }
};

class IntPassSemantics: public Z3Semantics {
public:
    IntPassSemantics(): Z3Semantics({TINT}, TINT, "int") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return inp[0];
    }

    virtual z3::expr getZ3Expr(std::vector<z3::expr>&& inp, z3::context& ctx, const std::vector<z3::expr>& params) {
        return inp[0];
    }
};

class AppendSemantics: public Semantics {
public:
    AppendSemantics(): Semantics({TLIST, TINT}, TLIST, "append") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        LIST x = inp[0].moveList(); int y = inp[1].getInt();
        x.push_back(y);
        return new ListValue(std::move(x));
    }
};

namespace semantics {
    Semantics* getSemanticsFromName(std::string name);
}

#endif //CPP_SEMANTICS_FACTORY_H
