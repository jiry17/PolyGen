#ifndef CPP_UNIFIER_H
#define CPP_UNIFIER_H

#include "program.h"
#include "bitset.h"

struct CmpInfo {
    Program* cmp;
    Bitset P, N;
    CmpInfo(Program* _cmp, const Bitset& _P, const Bitset& _N): cmp(_cmp), P(_P), N(_N) {}
};

namespace util {
    Program* mergeClause(const std::vector<Program*>& cmp_list);
    Program* mergeCondition(const std::vector<Program*>& clause_list);
}

class Unifier {
    std::vector<CmpInfo*> excludeDuplicated(const std::vector<Program*>& program_list);
protected:
    bool verifySolvable(const std::vector<CmpInfo*>& cmp_list);
public:
    std::vector<Program*> extra_list;
    std::vector<int> param_list;
    std::vector<PointExample*> P, N;
    std::vector<CmpInfo*> getCmpList(int cmp_size);
    virtual Program* getCondition(const std::vector<PointExample*>& positive_example, const std::vector<PointExample*>& negative_example) = 0;
    virtual std::string getType() = 0 ;
    virtual Program* unify(const std::vector<PointExample*>& example_list, const std::vector<Program*>& term_list, const std::vector<Program*>& pred_list) = 0;
};

#endif //CPP_UNIFIER_H
