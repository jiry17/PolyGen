#ifndef CPP_ENUMERATOR_H
#define CPP_ENUMERATOR_H

#include "grammar.h"
#include "program.h"

#include <unordered_set>

class Verifier {
public:
    virtual bool verify(Program* program) = 0;
};

class DefaultVerifier: public Verifier {
public:
    virtual bool verify(Program*) {return true;}
};

typedef std::vector<Program*> ProgramList;
typedef std::vector<ProgramList> ProgramStorage;

class Enumerator{
public:
    Grammar* grammar;
    Verifier* v;
    int size_upper_bound = 1e9;

    Enumerator(Grammar* _grammar): grammar(_grammar), v(new DefaultVerifier()) {}
    Enumerator(Grammar* _grammar, int _size_upper_bound):
            grammar(_grammar), v(new DefaultVerifier), size_upper_bound(_size_upper_bound) {}
    Enumerator(Grammar* _grammar, Verifier* _v): grammar(_grammar), v(_v) {}
    std::vector<Program*> enumerateProgram(int size_limit);
    std::vector<Program*> enumerateProgram(const std::vector<PointExample *>&example_list);
};
#endif //CPP_ENUMERATOR_H
