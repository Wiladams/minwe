#pragma once

#include "apidefs.h"
#include "maths.hpp"

#include <vector>

namespace alib
{
    template <typename T>
    struct Point
    {
        typedef T Type;
        T fX;
        T fY;

        Point();
        Point(const T& a, const T& b);

        template <typename TX, typename TY>
        Point(TX tx, TY ty);
        ~Point();

        // Copy construct from any point type
        //template <class TP, template<class> class TPoint> Point(const TPoint<TP>& p);


        T x() const;
        T y() const;

        // Assignment
        template <typename TP> Point& operator = (const Point<TP>& p);

        // Negation
        //Point<T> operator - (const Point<T>& p);

        // Some arithmetic operators
        // By some constant
        // multiply self by arbitrary type
        template <typename TA> Point& operator *= (const TA& a);
        Point& operator /= (double a);

        // By another point type
        template <typename TP> Point& operator += (const Point<TP>& p);
        template <typename TP> Point& operator -= (const Point<TP>& p);

    };


    // Field accessors
    template <typename T> T Point<T>::x() const { return fX; }
    template <typename T> T Point<T>::y() const { return fY; }

    // Declaration of operators
    template <typename T> Point<T> operator * (const Point<T>& p1, const Point<T>& p2);
    template <typename TP, typename TA> Point<TP> operator * (const Point<TP>& p, const TA& a);
    template <typename TP, typename TA> Point<TP> operator * (const TA& a, const Point<TP>& p);
    template <typename T> Point<T> operator - (const Point<T>& p);

    // Struct Point Implementation
    // Point is one of the most used data structures.  We want
    // want it to be efficient, and comlplete for conversions
    template <class TD, class TS>
    INLINE TD Convert(TS src)
    {
        return (TD)src;
    }

    template<>
    INLINE ptrdiff_t Convert<ptrdiff_t, double>(double src)
    {
        return (ptrdiff_t)maths::Round(src);
    }

    template<>
    INLINE ptrdiff_t Convert<ptrdiff_t, float>(float src)
    {
        return (ptrdiff_t)maths::Round(src);
    }

    template <typename T>
    Point<T>::Point() : fX(T()), fY(T()) {}

    template <typename T>
    Point<T>::Point(const T& x, const T& y)
        : fX(x), fY(y) {}

    // This constructor allows you to create
    // a point with a mix of types from double,
    // float, and int typically
    template <typename T>
    template <typename TX, typename TY>
    INLINE Point<T>::Point(TX tx, TY ty)
        :fX(Convert<T, TX>(tx))
        , fY(Convert<T, TY>(ty))
    {}

    template <typename T>
    Point<T>::~Point() {}

    template <typename T> template <typename TP>
    INLINE Point<T>& Point<T>::operator = (const Point<TP>& p)
    {
        fX = Convert<T, TP>(p.fX);
        fY = Convert<T, TP>(p.fY);
        return *this;
    }

    template <typename T> template <typename TA>
    INLINE Point<T>& Point<T>::operator *= (const TA& a)
    {
        fX = Convert<T, TA>(fX * a);
        fY = Convert<T, TA>(fY * a);
        return *this;
    }

    template <typename T>
    INLINE Point<T>& Point<T>::operator /= (double a)
    {
        fX = Convert<T, double>(fX / a);
        fY = Convert<T, double>(fY / a);
        return *this;
    }

    template <typename T> template <typename TP>
    INLINE Point<T>& Point<T>::operator += (const Point<TP>& p)
    {
        fX += Convert<T, TP>(p.fX);
        fY += Convert<T, TP>(p.fY);
        return *this;
    }

    template <typename T> template <typename TP>
    INLINE Point<T>& Point<T>::operator -= (const Point<TP>& p)
    {
        fX -= Convert<T, TP>(p.fX);
        fY -= Convert<T, TP>(p.fY);
        return *this;
    }

    template <typename T>
    INLINE Point<T> operator + (const Point<T>& p1, const Point<T>& p2)
    {
        return Point<T>(p1.fX + p2.fX, p1.fY + p2.fY);
    }

    // Negation
    template <typename T>
    INLINE Point<T> operator - (const Point<T>& p)
    {
        return Point<T>(-p.FX, -p.fY);
    }

    // Subtraction
    template <typename T>
    INLINE Point<T> operator - (const Point<T>& p1, const Point<T>& p2)
    {
        return Point<T>(p1.fX - p2.fX, p1.fY - p2.fY);
    }

    // Multiplication
    template <typename T>
    INLINE Point<T> operator * (const Point<T>& p1, const Point<T>& p2)
    {
        return Point<T>(p1.fX * p2.fX, p1.fY * p2.fY);
    }

    template <typename TP, typename TA>
    INLINE Point<TP> operator * (const Point<TP>& p, const TA& a)
    {
        return Point<TP>(p.fX * a, p.fY * a);
    }

    template <typename TP, typename TA>
    INLINE Point<TP> operator * (const TA& a, const Point<TP>& p)
    {
        return Point<TP>(p.fX * a, p.fY * a);
    }




    template <typename T>
    struct Geometry
    {
        virtual bool contains(const T& x, const T& y) const { return false; }
    };

    // Axis aligned bounding box
    template <typename T>
    struct AABB
    {
        T left;
        T top;
        T right;
        T bottom;
    };

    // A starting of a line and a length
    template <typename T>
    struct GeoSpan
    {
    public:
        T fX;
        T fY;
        T fW;

    public:
        INLINE GeoSpan() noexcept = default;
        INLINE constexpr GeoSpan(const GeoSpan<T>& other) noexcept = default;
        INLINE GeoSpan(T x, T y, T w) noexcept :fX(x), fY(y), fW(w) {}

        INLINE GeoSpan& operator=(const GeoSpan<T>& other) noexcept = default;

        INLINE constexpr T x() const noexcept { return fX; }
        INLINE constexpr T y() const noexcept { return fY; }
        INLINE constexpr T w() const noexcept { return fW; }

        INLINE constexpr T rightMost() const noexcept { return fX + fW - 1; }
    };

    // Representation of a line segment
    template <typename T>
    struct GeoLine
    {
        Point<T> pt1;
        Point<T> pt2;
    };

    // General rectangle representation
    //
    template <typename T>
    struct GeoRect : public Geometry<T>
    {
    public:
        T fX;
        T fY;
        T fWidth;
        T fHeight;

    public:
        GeoRect() = default;
        GeoRect(T x, T y, T w, T h) :fX(x), fY(y), fWidth(w), fHeight(h) {}

        INLINE T constexpr x()const { return fX; }
        INLINE T constexpr y()const { return fY; }
        INLINE T constexpr w()const { return fWidth; }
        INLINE T constexpr h()const { return fHeight; }

        INLINE T constexpr left() const { return fX; }
        INLINE T constexpr top() const { return fY; }
        INLINE T constexpr right() const { return fX + fWidth; }
        INLINE T constexpr bottom() const { return fY + fHeight; }

        INLINE Point<T> center() const { return Point<T>({ fX + fWidth / 2.0, fY + fHeight / 2.0 }); }

        INLINE bool isEmpty() const { return (fWidth <= 0) || (fHeight <= 0); }

        INLINE bool contains(const T& x, const T& y) const override
        {
            if ((x < fX) || (y < fY))
                return false;

            if ((x >= (fX + fWidth)) || (y >= (fY + fHeight)))
                return false;

            return true;
        }

        // See if another rectangle is completely contained within us
        INLINE constexpr bool contains(const GeoRect<T>& other) const
        {
            if (!contains(other.x, other.y))
            {
                return false;
            }

            if (!contains(other.x + other.width - 1, other.y + other.height - 1))
            {
                return false;
            }

            return true;
        }

        INLINE GeoRect<T> intersection(const GeoRect<T>& b) const
        {
            T x = fX > b.x() ? fX : b.x();
            T y = fY > b.y() ? fY : b.y();
            T right = ((fX + w()) < (b.fX + b.w())) ? (fX + w()) : (b.fX + b.w());
            T bottom = ((fY + h()) < (b.fY + b.h())) ? (fY + h()) : (b.fY + b.h());

            T width = ((right - x) > 0) ? (right - x) : 0;
            T height = ((bottom - y) > 0) ? (bottom - y) : 0;

            return GeoRect<T>(x, y, width, height);
        }

        INLINE GeoRect<T>& operator=(const GeoRect<T>& rhs) noexcept
        {
            fX = rhs.fX;
            fY = rhs.fY;
            fWidth = rhs.fWidth;
            fHeight = rhs.fHeight;

            return *this;
        };

        // The arithmetic operators are good for doing
        // interpolation
        GeoRect<T>& operator+=(const GeoRect<T>& other)
        {
            fX += other.fX;
            fY += other.fY;
            fWidth += other.fWidth;
            fHeight += other.fHeight;

            return *this;
        }

        GeoRect<T>& operator *= (double s)
        {
            fX *= s;
            fY *= s;
            fWidth *= s;
            fHeight *= s;

            return *this;
        }

        GeoRect<T> operator+(const GeoRect<T>& rhs);
        GeoRect<T> operator * (double s) const;
    };

    // GeoRect<T> Implementation
    //
    template <typename T>
    GeoRect<T> GeoRect<T>::operator *(double s) const
    {
        GeoRect<T> res(*this);
        return res *= s;
    }

    template <typename T>
    GeoRect<T> GeoRect<T>::operator+(const GeoRect<T>& rhs)
    {
        GeoRect<T> res(*this);
        return res += rhs;
    }



    template <typename T>
    struct GeoPolygon
    {
        ptrdiff_t fTop = 65535;
        ptrdiff_t fBottom = 0;
        bool fIsClosed = true;
        std::vector<alib::Point<T> > fVertices;

        GeoPolygon(bool closed = false)
            :fTop(65535)
            , fBottom(0)
            , fIsClosed(closed)
        {
            clear();
        }

        bool isClosed() { return fIsClosed; }
        bool isEmpty() { return fVertices.size() == 0; }

        void setClose(bool toClose) { fIsClosed = toClose; }

        // clear out existing commands and vertices
        void clear()
        {
            fVertices.clear();
            fTop = 65535;
            fBottom = 0;
        }

        GeoRect<T> getBounds()
        {
            T minx = 65535;
            T maxx = -65535;
            T miny = 65535;
            T maxy = -65535;

            for (auto& pt : fVertices)
            {
                minx = maths::Min(pt.x(), minx);
                maxx = maths::Max(pt.x(), maxx);
                miny = maths::Min(pt.y(), miny);
                maxy = maths::Max(pt.y(), maxy);
            }

            return GeoRect<T>(minx, miny, maxx - minx, maxy - miny);
        }

        void addPoint(const Point<T>& pt)
        {
            fVertices.push_back(pt);
        }

        void findTopmost()
        {
            fTop = 65535;
            fBottom = 0;

            for (size_t i = 0; i < fVertices.size(); ++i)
            {
                fTop = fTop < fVertices[i].y() ? fTop : fVertices[i].y();
                fBottom = fBottom > fVertices[i].y() ? fBottom : fVertices[i].y();
                //fTop = std::min(fTop, fVertices[i].y());
                //fBottom = std::max(fBottom, fVertices[i].y());
            }
            fTop = std::max<ptrdiff_t>(0, fTop);
            fBottom = std::min<ptrdiff_t>(fBottom, 65535);
        }
    };

    /*
   GeoTriangle

   A triangle is a specialization of a polygon
 */
    template <typename T>
    struct GeoTriangle : public GeoPolygon<T>
    {

        // Basic constructor take coordinates in any order, sorts them
        // so that subsequent drawing will be faster.
        GeoTriangle(const T x1, const T y1,
            const T x2, const T y2,
            const T x3, const T y3)
            :GeoPolygon<T>(true)
        {
            GeoPolygon<T>::addPoint(Point<T>(x1, y1));
            GeoPolygon<T>::addPoint(Point<T>(x2, y2));
            GeoPolygon<T>::addPoint(Point<T>(x3, y3));

            GeoPolygon<T>::findTopmost();
        }
    };

    // GeoEllipse Definition
    template <typename T>
    struct GeoEllipse : public GeoPolygon<T>
    {
        static constexpr size_t nverts = 72;   // should specialize for size

        size_t steps = nverts;
        T cx, cy;   // center of ellipse
        T rx;       // radius in x axis
        T ry;       // radius in y axis

        GeoEllipse(T cx_, T cy_, T rx_, T ry_)
            :GeoPolygon<T>(true)
            , cx(cx_)
            , cy(cy_)
            , rx(rx_)
            , ry(ry_)
        {
            init(cx_, cy_, rx_, ry_);
        }

        bool init(T cx_, T cy_, T xRadius, T yRadius)
        {
            T awidth = xRadius * 2;
            T aheight = yRadius * 2;

            for (size_t i = 0; i < steps; i++)
            {
                auto u = (double)i / steps;
                auto angle = u * (2 * maths::Pi);

                T x = (T)floor((awidth / 2.0) * cos(angle));
                T y = (T)maths::Floor((aheight / 2.0) * sin(angle));
                addPoint(Point<T>(x + cx_, y + cy_));
            }

            // prepare for drawing
            this->findTopmost();
        }
    };


    template <typename T>
    struct GeoCircle : public Geometry<T>
    {
        T fX, fY;
        T fR;

        GeoRect<T> getBoundingBox() override
        {
            auto x1 = fX - fR, y1 = fY - fR;
            auto w = fR * 2;

            return GeoRect<T>(x1, y1, w, w);
        }

        T cx() { return fX; }
        T cy() { return fY; }
        T r() { return fR; }
    };




    // Bezier Reference
    // https://pages.mtu.edu/~shene/COURSES/cs3621/NOTES/spline/Bezier/bezier-construct.html
    // https://pomax.github.io/bezierinfo/#tracing
    //
    // Quadratic Bezier, defined by 3 points
    template <typename T>
    struct GeoBezier
    {
        std::vector<T> tvals;

        virtual Point<T> eval(const double u) const = 0;

        // generate a list of 't' values that will give 
        // equal distance
        INLINE double findUForX(ptrdiff_t x) const
        {
            double highEnd = 1.0;
            double lowEnd = 0.0;

            // Binary search to find the solution
            while (true)
            {
                double u = highEnd - ((highEnd - lowEnd) / 2.0);

                ptrdiff_t evalX = eval(u).fX;

                if (evalX == x)
                    return u;

                if (evalX > x)
                {
                    highEnd = u;
                }
                else if (evalX < x)
                {
                    lowEnd = u;
                }
            }
        }

        // Find t values that match x values
        INLINE void calcSpeeds()
        {
            // Figure out lowest value for t==0
            // Figure out highest value for t == 1.0

            auto p1 = eval(0);
            auto p2 = eval(1.0);

            tvals.empty();

            // do the loop searching for best fits
            for (size_t x = p1.x(); x <= p2.x(); x++)
            {
                auto u = findUForX(x);

                tvals.push_back(u);
            }
        }
    };


    template <typename T>
    struct GeoQuadraticBezier : public GeoBezier<T>
    {
        Point<T> p1;
        Point<T> c1;
        Point<T> p2;

    public:
        GeoQuadraticBezier(const Point<T>& pp1, const Point<T>& pp2, const Point<T>& pp3)
            :p1(pp1), c1(pp2), p2(pp3)
        {}

        GeoQuadraticBezier(const T x1, const T y1, const T c1x, const T c1y, const T x3, const T y3)
            :p1(x1, y1)
            , c1(c1x, c1y)
            , p2(x3, y3)
        {}

        // Value of curve at parametric position 'u'
        Point<T> eval(const double u) const
        {
            double oneminusu = 1.0 - u;
            return c1 + ((oneminusu * oneminusu) * (p1 - c1)) + ((u * u) * (p2 - c1));
        }
    };

    // Cubic Bezier, defined by 4 points
    template <typename T>
    struct GeoCubicBezier : public GeoBezier<T>
    {
        Point<T> p1;
        Point<T> p2;
        Point<T> p3;
        Point<T> p4;

    public:

    public:
        GeoCubicBezier(const Point<T>& pp1, const Point<T>& pp2, const Point<T>& pp3, const Point<T>& pp4)
            :p1(pp1), p2(pp2), p3(pp3), p4(pp4)
        {}

        GeoCubicBezier(const T x1, const T y1, const T x2, const T y2, const T x3, const T y3, const T x4, const T y4)
            :p1({ x1, y1 })
            , p2({ x2, y2 })
            , p3({ x3, y3 })
            , p4({ x4, y4 })

        {}


        // Value of curve at parametric position 'u'
        // control points are P0, P1, P2, P3
        Point<T> eval(const double u) const
        {
            double oneminusu = 1 - u;
            double BEZ03 = oneminusu * oneminusu * oneminusu;	// (1-u)^3
            double BEZ13 = 3 * u * (oneminusu * oneminusu);	    // 3u(1-u)^2
            double BEZ23 = 3 * (u * u) * oneminusu;				// 3u^2(1-u)
            double BEZ33 = u * u * u;							// u^3

            return ((p1 * BEZ03) + (p2 * BEZ13) + (p3 * BEZ23) + (p4 * BEZ33));
        }

    };


}
// alib namespace