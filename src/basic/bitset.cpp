#include "bitset.h"

#include <cassert>
#include <iostream>

void Bitset::append(unsigned int k) {
#ifdef DEBUG
    assert(k == 0 || k == 1);
#endif
    if ((n & 31u) == 0) A.push_back(k);
    else if (k) A[n >> 5u] |= (1u << (n & 31u));
    ++n;
}

bool Bitset::operator<(const Bitset &x) const {
    if (n < x.n) return true; else if (n > x.n) return false;
    return A < x.A;
}

int Bitset::count() const {
    int ans = 0;
    for (auto& w: A) ans += __builtin_popcount(w);
    return ans;
}

Bitset Bitset::operator&(const Bitset &x) const{
    std::vector<unsigned int> result(A);
#ifdef DEBUG
    assert(x.n == n);
#endif
    for (int i = 0; i < A.size(); ++i) {
        result[i] &= x.A[i];
    }
    return Bitset(std::move(result), n);
}

Bitset Bitset::operator~() const {
    std::vector<unsigned int> result(A);
    for (int i = 0; i < A.size(); ++i) {
        result[i] = ~result[i];
    }
    if (n & 31) {
        int last = result.size() - 1;
        unsigned int rem = n & 31;
        result[last] &= ((1u << rem) - 1);
    }
    return Bitset(std::move(result), n);
}

Bitset Bitset::operator|(const Bitset &x) const{
    std::vector<unsigned int> result(A);
#ifdef DEBUG
    assert(x.n == n);
#endif
    for (int i = 0; i < A.size(); ++i) {
        result[i] |= x.A[i];
    }
    return Bitset(std::move(result), n);
}

Bitset Bitset::exclude(const Bitset &x) const {
    std::vector<unsigned int> result(A);
#ifdef DEBUG
    assert(x.n ==n);
#endif
    for (int i = 0; i < A.size(); ++i) {
        result[i] = result[i] & (~x.A[i]);
    }
    return Bitset(std::move(result), n);
}

bool Bitset::checkCover(const Bitset &x) const {
#ifdef DEBUG
    assert(x.n == n);
#endif
    // std::cout << "cover " << A.size() << " " << x.A.size() << std::endl;
    for (int i = 0; i < A.size(); ++i) {
        if ((A[i] & x.A[i]) != x.A[i]) return false;
    }
    return true;
}

std::string Bitset::toString() {
    std::string result = "";
    for (int i = 0; i < n; ++i) result += std::to_string((*this)[i]);
    return result;
}

bool Bitset::operator[](unsigned int k) const {
    return (A[k >> 5u] >> (k & 31u)) & 1u;
}

void Bitset::set(unsigned int pos, bool c) {
    assert(0 <= pos && pos < n);
    int where = pos >> 5u;
    unsigned int v = 1u << (pos & 31u);
    if (!c && (A[where] & v)) A[where] -= v;
    else if (c && !(A[where] & v)) A[where] += v;
}

Bitset::Bitset(unsigned int _n, bool c): n(_n) {
    unsigned int m = n >> 5u;
    A.resize(m);
    for (int i = 0; i < m; ++i) {
        A[i] = c ? -1 : 0;
    }
    if ((n & 31u)) {
        if (c) A.push_back((1u << (n & 31u)) - 1); else A.push_back(0);
    }
}