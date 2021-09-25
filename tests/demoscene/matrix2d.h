#pragma once

/*
    A matrix object with Postscript transformation
    behavior and representation.  It is essentially
    a truncated 3x3 matrix.

    a   b  =>   m00  m01  0
    c   d  =>   m10  m11  0
    tx ty  =>   m20  m21  1
*/

#include <math.h>


struct Matrix2D {
    union {
        double m[6];
        struct {
            double m00;
            double m01;
            double m10;
            double m11;
            double m20;
            double m21;
        };
    };

    // Default Constructor
    Matrix2D() :m{ 0 } {}

    // Copy constructor
    Matrix2D(const Matrix2D& m)
    {
        this->m[0] = m.m[0];
        this->m[1] = m.m[1];
        this->m[2] = m.m[2];
        this->m[3] = m.m[3];
        this->m[4] = m.m[4];
        this->m[5] = m.m[5];
    }

    // Array MUST contain 6 elements
    static Matrix2D createFromArray(const double arr[])
    {
        Matrix2D m;
        m.m[0] = arr[0];
        m.m[1] = arr[1];
        m.m[2] = arr[2];
        m.m[3] = arr[3];
        m.m[4] = arr[4];
        m.m[5] = arr[5];

        return m;
    }

    static Matrix2D createIdentity()
    {
        double arr[] = { 1,0,0,1,0,0 };
        return createFromArray(arr);
    }

    static Matrix2D createTranslation(double tx, double ty)
    {
        double arr[] = { 1,0,0,1,tx,ty };
        return createFromArray(arr);
    }

    /*
        -- rotation counter clockwise about origin by an
        -- angle specified in radians
             cos(a)  sin(a)  0
            -sin(a)  cos(a)  0
              0       0      1
    */
    static Matrix2D createRotation(double rads, double cx, double cy)
    {

        //rads = math.rad(angle);
        double rcos = cos(rads);
        double rsin = sin(rads);

        double arr[] = { rcos, rsin, -rsin, rcos, cx,cy };
        return createFromArray(arr);
    }

    // Create a matrix with scaling only
    static Matrix2D createScaling(double sx, double sy)
    {
        double arr[] = { sx, 0, 0, sy, 0, 0 };
        return createFromArray(arr);
    }

    // Return a new matrix that is the inverse
    // of the current matrix
    Matrix2D inverse()
    {
        // print("createInverse, self:")
        double d = determinant();

        // if determinant is 0 there is no inverse
        if (d == 0) {
            // BUGBUG - should probably throw exception
            return createIdentity();
        }

        //print("matrix.inverse, d: ", d)
        // create some temporaries
        double t00 = this->m11;
        double t01 = -this->m01;
        double t10 = -this->m10;
        double t11 = this->m00;

        t00 = t00 / d;
        t01 = t01 / d;
        t10 = t10 / d;
        t11 = t11 / d;

        double t20 = -(this->m20 * t00 + this->m21 * t10);
        double t21 = -(this->m20 * t01 + this->m21 * t11);

        double arr[] = { t00,t01,t10,t11,t20,t21 };
        return createFromArray(arr);
    }

    // used to determine if we can create an inverse
    double determinant()
    {
        return this->m00 * this->m11 - this->m01 * this->m10;
    }

    bool transformPoint(double &x, double &y)
    {
        double x1 = this->m00 * x + this->m10 * y + this->m20;
        double y1 = this->m01 * x + this->m11 * y + this->m21;

        x = x1;
        y = y1;

        return true;
    }
};










/*
//    rotate
//    apply rotation to current transform
//    maintain scaling and translation
    rotate = function(self, angle, cx, cy)
        -- create rotation matrix
        local r = PSMatrix:createRotation(angle, cx, cy)

        -- premultiply by current
        self:preMultiplyBy(r)

        return self
    end;

    -- scale
    -- apply scaling to current matrix,
    -- keeping rotation and translation intact
    scale = function(self, sx, sy)
        self.m00 = self.m00 * sx;
        self.m01 = self.m01 * sx;
        self.m10 = self.m10 * sy;
        self.m11 = self.m11 * sy;

        return self
    end;

    -- translate
    -- apply matrix transform to specified offsets
    -- and add to current matrix translation
    translate = function(self, tx, ty)
        local x1,y1 = self:transformPoint(tx, ty)
        self.m20 = self.m20 + x1
        self.m21 = self.m21 + y1

        return self
    end;
    */
/*
    -- concatenate matrix transformations
    -- m = other * m
    -- a   b  0     a  b  0
    -- c   d  0     c  d  0
    -- tx ty  1    tx ty  1
    preMultiplyBy = function(self, other)
        local a = other.m00*self.m00 + other.m01*self.m10;
        local b = other.m00*self.m01 + other.m01*self.m11;

        local c = other.m10*self.m00 + other.m11*self.m10;
        local d = other.m10*self.m01 + other.m11*self.m11;

        local tx = other.m20*self.m00 + other.m21*self.m10 + self.m20;
        local ty = other.m20*self.m01 + other.m21*self.m11 + self.m21;

        self.m00 = a
        self.m01 = b
        self.m10 = c
        self.m11 = d
        self.m20 = tx
        self.m21 = ty

        return self
    end;
*/
/*
    dtransform = function(self, x, y)
        local x1 = self.m00*x + self.m10*y
        local y1 = self.m01*x + self.m11*y

        return x1, y1
    end;
*/



/*
    -- Take the matrix, and only apply the scaling portion
    scalePoint = function(self, x, y)
        local m1 = PSMatrix:createScaling(self.m00, self.m11)
        return m1:transformPoint(x,y)
    end;
*/

