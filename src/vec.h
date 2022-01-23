#pragma once

/*
    'vector' data structures.  Once more with feeling this time.
    This is probably the most often implemented library
    I've done over the past 35 years.  Some times it's the exact
    same, sometimes there are slight variants.  In this iteration
    I'm trying to use minimal code, keep it all in one header
    and only implement the bare necessities.

    Hopefully the routines implemented here are rational
    enough that individual programs don't have a need to 
    implement their own specializations.

    Embrace templates and the standard library

    Notes:
    Look at GLM math library
    Eigen3

*/
#include "apidefs.h"
#include "maths.hpp"

#include <cassert>
#include <cmath>
#include <array>


//
// vec<DIM, T>
//
// A base class implementation that does the basic 
// data structure of a vector, implements only a few
// common things
// addition - two vectors, or a scalar
// subtraction - two vectors, or a scalar
// multiplication - by a scalar
//
template <size_t DIM, typename T>
struct vec
{
    std::array<T, DIM> data;

    INLINE vec() = default;
    INLINE vec(const vec<DIM,T>& other) = default;
    INLINE vec(const std::array<T, DIM>& other) : data(other) {}

    // some convenient data returns
    T u() const { return data[0]; }
    T v() const { return data[1]; }

    T s() const { return data[0]; }
    T t() const { return data[1]; }

    T x() const { return data[0]; }
    T y() const { return data[1]; }
    T z() const { return data[2]; }
    T w() const { return data[3]; }

    T r() const { return data[0]; }
    T g() const { return data[1]; }
    T b() const { return data[2]; }
    T a() const { return data[3]; }


    // Assignment operator
    INLINE vec<DIM, T> & operator=(const std::array<T, DIM>& rhs) {data = rhs;return *this;}

    // index operators
    INLINE const T operator[](const size_t i) const { assert(i < DIM);  return data[i]; } // can't change the value
    INLINE T& operator[](const size_t i) { assert(i < DIM);  return data[i]; }                // allows you to change the value

    // Do NOT implement these overloads here
    // they have special meanings for vectors
    // the multiplication, for example, is the Hadamard transform
    vec<DIM, T> operator * (const vec<DIM, T>& v) const = delete;
    vec<DIM, T> operator / (const vec<DIM, T>& v) const = delete;

    // Negation operator
    INLINE vec<DIM, T> operator-()const { 
        vec<DIM, T> res;
        for (size_t i = 0; i < DIM; i++)
            res[i] = -data[i];

        return res; 
    }

    // self addition
    INLINE vec<DIM, T>& operator+=(const vec<DIM, T> & rhs)
    {
        for (size_t i = 0; i < DIM; i++)
            data[i] += rhs[i];
        return *this;
    }

    // self subtraction
    INLINE vec<DIM, T>& operator-=(const vec<DIM, T> & rhs)
    {
        for (size_t i = 0; i < DIM; i++)
            data[i] -= rhs[i];

        return *this;
    }

    // Multiply self by scalar
    INLINE vec<DIM, T>& operator*=(const double s)
    {
        for (auto &it : data)
            it *= s;

        return *this;
    }

    // divide self by scalar
    INLINE vec<DIM, T>& operator/=(const double s)
    {
        for (auto& it : data)
            it /= s;

        return *this;
    }

    // New instance operations 
// Operators creating a new instance of vector
    INLINE  vec<DIM,T> operator + (const vec<DIM, T>& rhs) const { vec<DIM, T> res(*this); return res += rhs; }
    INLINE  vec<DIM, T> operator - (const vec<DIM, T>& rhs) const { vec<DIM, T> res(*this); return res -= rhs; }

    // Multiply by a scalar
    INLINE  vec<DIM, T> operator * (const double s) const { vec<DIM, T> res(*this); return res *= s; }
    INLINE  vec<DIM, T> operator / (const double s) const { vec<DIM, T> res(*this); return res /= s; }


    // Convenience for length() calculation
    INLINE double lengthSquared() const {
        double res = 0;
        for (size_t i = 0; i < DIM; i++)
            res += data[i] * data[i];

        return res;
    }

    INLINE double length() const {return std::sqrt(lengthSquared());}

    INLINE vec<DIM,T> unit() const { return *this / length(); }

    // Return true if the vector is close to zero in all dimensions.
    bool near_zero() const {
        const auto s = 1e-8;
        for (size_t i = 0; i < DIM; i++) {
            if (std::fabs(data[i]) > s)
                return false;
        }

        return true;
    }

};

template <size_t DIM, typename T>
INLINE vec<DIM,T> operator *(double s, vec<DIM, T> rhs) { return rhs * s; }

// The dot product is often implemented
// by overriding the operator *, but we want to leave
// it as an explicit function so whomever is looking later
// is not confusing this with another function
template <size_t DIM, typename T>
INLINE double dot(const vec<DIM, T>& u, const vec<DIM, T>& v)
{
    double res = 0;
    for (size_t i = 0; i < DIM; i++)
        res += u[i] * v[i];

    return res;
}

// The cross product is sometimes implemented as the '^' operator
// we'll leave as an explicit function call so avoid confusion
template <typename T>
INLINE vec<3,T> cross(const vec<3,T>& u, const vec<3,T>& v) 
{
    return vec<3, T>({ 
        u[1] * v[2] - u[2] * v[1],
        u[2] * v[0] - u[0] * v[2],
        u[0] * v[1] - u[1] * v[0] });
}

template <typename T>
struct vec2 : public vec<2,T> 
{
    vec2(const T&a, const T&b)
        : vec<2, T>({ a,b }) {}

};

using vec2i = vec<2, int>;
using vec2f = vec<2, float>;

using vec3f = vec<3, float>;


using vec4f = vec<4, float>;
