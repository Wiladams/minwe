#pragma once


//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include <cmath>
#include <iostream>

#include "vec.h"

/*
class vec3 {
  public:
    vec3() : e{0,0,0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    // negation operator
    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }

    // index operators
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    // a = a + b
    vec3& operator+=(const vec3 &v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    //
    // a = a * t
    vec3& operator*=(const double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    // a = a/t
    vec3& operator/=(const double t) {
        return *this *= 1.0/t;
    }

    double length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    double length() const {
        return std::sqrt(length_squared());
    }



    // Return true if the vector is close to zero in all dimensions.
    bool near_zero() const {
        
        const auto s = 1e-8;
        return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
    }

    // generate random vector
    static vec3 random() {
        return vec3(maths::random_double(), maths::random_double(), maths::random_double());
    }

    // generate random vector within a range
    static vec3 random(double min, double max) {
        return vec3(maths::random_double(min,max), maths::random_double(min,max), maths::random_double(min,max));
    }

  public:
    double e[3];
};
*/




// vec3 Utility Functions
using vec3 = vec<3, double>;

inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
    return out << v[0] << ' ' << v[1] << ' ' << v[2];
}

/*
inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t) {
    return t * v;
}

inline vec3 operator/(vec3 v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3 &u, const vec3 &v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) 
{
    return v / v.length();
}
*/

inline vec3 random_in_unit_disk();
inline vec3 random_in_unit_sphere();
inline vec3 random_unit_vector();
inline vec3 random_in_hemisphere(const vec3& normal);
inline vec3 reflect(const vec3& v, const vec3& n);
inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat);
inline vec3 random_cosine_direction();



// Type aliases for vec3
using point3 = vec<3, double>;   // 3D point
using color = vec<3, double>;    // RGB color
