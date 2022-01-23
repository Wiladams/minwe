#pragma once

//==============================================================================================
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

class interval {
  public:
    interval() : min(+maths::infinity), max(-maths::infinity) {} // Default interval is empty

    interval(double _min, double _max) : min(_min), max(_max) {}

    interval(const interval& a, const interval& b)
      : min(fmin(a.min, b.min)), max(fmax(a.max, b.max)) {}

    double size() const {
        return max - min;
    }

    interval expand(double delta) const {
        const auto padding = delta/2;
        return interval(min - padding, max + padding);
    }

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    static const interval empty, universe;

  public:
    double min, max;
};


interval operator+(const interval& ival, double displacement);
interval operator+(double displacement, const interval& ival);




