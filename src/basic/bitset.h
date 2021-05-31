#ifndef CPP_BITSET_H
#define CPP_BITSET_H

#include <vector>
#include <algorithm>
#include <string>
class Bitset {
    std::vector<unsigned int> A;
    unsigned int n;
public:
    std::string toString();
    Bitset(std::vector<unsigned int>&& _A, unsigned int _n): A(std::move(_A)), n(_n) {}
    Bitset(): n(0) {}
    Bitset(unsigned int n, bool c);
    int count() const;
    void append(unsigned int k);
    void set(unsigned int pos, bool c);
    Bitset operator | (const Bitset& x) const;
    Bitset operator & (const Bitset& x) const;
    Bitset operator ~ () const;
    Bitset exclude (const Bitset& x) const;
    bool checkCover(const Bitset& x) const;
    bool operator [] (unsigned int k) const;
    bool operator < (const Bitset& x) const;
    int size() const {return n;}
};

#endif //CPP_BITSET_H
