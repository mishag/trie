#ifndef INCLUDED_TRIEKEYITERATOR_H
#define INCLUDED_TRIEKEYITERATOR_H

#include <string>


    // =====================
    // class IntegralKeyIter
    // =====================

template <typename T, unsigned int BASE>
class IntegralKeyIter {
private:
    typedef unsigned long long UInt64;
    
    UInt64  d_current;
    bool    d_valid;

public:
    explicit IntegralKeyIter(T key)
        : d_current((UInt64)key)
        , d_valid(true)
    {
    }

    ~IntegralKeyIter() {}

    operator bool() const 
    {
        return d_valid;
    }

    IntegralKeyIter& operator++()
    {
        if (!d_valid) {
            throw std::runtime_error("Iterator out of range.");
        }

        d_current /= BASE;
        d_valid = (d_current != 0);

        return *this;
    }

    size_t value() const { return d_current % BASE; }
};

    // ===================
    // class StringKeyIter
    // ===================

class StringKeyIter {
private:
    const char   *d_string;
    const char   *d_pos;
    char          d_curChar;
    bool          d_valid;
    bool          d_shouldAdvance;

public:
    explicit StringKeyIter(const char * str)
        : d_string(str)
        , d_pos(str)
        , d_curChar(0)
        , d_valid(d_string != 0 && *d_string != 0)
        , d_shouldAdvance(false)
    {
        if (!d_valid) {
            return;
        }

        d_curChar = *d_pos;
    }

    explicit StringKeyIter(const std::string& str)
        : d_string(str.c_str())
        , d_pos(d_string)
        , d_curChar(0)
        , d_valid(!str.empty())
        , d_shouldAdvance(false)
    {
        if (!d_valid) {
            return;
        }

        d_curChar = *d_pos;
    }

    operator bool() const { return d_valid; }

    StringKeyIter& operator++()
    {
        if (!d_valid) {
            throw std::runtime_error("Iterator out of range.");
        }

        if (!d_shouldAdvance) {
            d_shouldAdvance = true;
            return *this;
        }

        ++d_pos;
        d_curChar = *d_pos;
        d_valid = (0 != *d_pos);
        d_shouldAdvance = false;

        return *this;
    }

    size_t value() const
    {
        return d_shouldAdvance ? (d_curChar % 16) : (d_curChar / 16);
    }

    ~StringKeyIter() {}
};

#endif
