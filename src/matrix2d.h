#pragma once

#include "maths.hpp"

/*
	A PSMatrix object with Postscript transformation
	behavior and representation.  It is essentially
	a truncated 3x3 matrix.

	a	b	==>	m00  m01  0
	c	d	==> m10  m11  0
	tx ty	==> m20  m21  1
*/

// BUGBUG - would it be better to use std::variant?
struct Matrix2D {
	union {
		struct {
			double m00;
			double m01;
			double m10;
			double m11;
			double m20;
			double m21;
		};
		double m[6];
	};

	//
	// default constructor
	Matrix2D()
	{
		for (size_t i = 0; i < 6; i++)
		{
			m[i] = 0.0;
		}
	}

	Matrix2D(const double d0, const double d1, const double d2, const double d3, const double d4, const double d5)
	{
		m[0] = d0;
		m[1] = d1;
		m[2] = d2;
		m[3] = d3;
		m[4] = d4;
		m[5] = d5;
	}

	/*
		Copy constructor - clone
	*/
	Matrix2D(const Matrix2D& other)
	{
		for (size_t i = 0; i < 6; i++)
		{
			m[i] = other.m[i];
		}
	}
	/*
	// Create a matrix from a Postscript Array
	// The array contains token values
	static inline PSMatrix createFromArray(const PSArray& arr)
	{
		PSMatrix m{ arr[0]->asDouble(),
			arr[1]->asDouble(),
			arr[2]->asDouble(),
			arr[3]->asDouble(),
			arr[4]->asDouble(),
			arr[5]->asDouble() };

		return m;
	}
	*/
	// Factory constructors
	static inline Matrix2D createIdentity()
	{
		Matrix2D m{ 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 };

		return m;
	}

	static inline Matrix2D createTranslation(const double tx, const double ty)
	{
		Matrix2D m{ 1.0, 0.0, 0.0, 1.0, tx, ty };

		return m;
	}

	/*
		Rotation counter clockwise about a point
		by an angle specified in degrees

			cos(a)  sin(a)  0
		   -sin(a)  cos(a)  0
			 0       0      1
	*/
	static inline Matrix2D createRotation(const double angle, const double cx = 0, const double cy = 0)
	{
		auto rads = maths::Radians(angle);
		auto rcos = maths::Cos(rads);
		auto rsin = maths::Sin(rads);

		Matrix2D m{ rcos, rsin, -rsin, rcos, cx, cy };
		return m;
	}

	static inline Matrix2D createScaling(const double sx, const double sy)
	{
		Matrix2D m{ sx, 0.0, 0.0, sy, 0.0, 0.0 };
		return m;
	}


	//
	// Instance Methods
	//

	// preMultiplyBy
	// this = other * this
	//
	Matrix2D& preMultiplyBy(const Matrix2D& other)
	{
		auto a = other.m00 * m00 + other.m01 * m10;
		auto b = other.m00 * m01 + other.m01 * m11;

		auto c = other.m10 * m00 + other.m11 * m10;
		auto d = other.m10 * m01 + other.m11 * m11;

		auto tx = other.m20 * m00 + other.m21 * m10 + m20;
		auto ty = other.m20 * m01 + other.m21 * m11 + m21;

		m00 = a;
		m01 = b;
		m10 = c;
		m11 = d;
		m20 = tx;
		m21 = ty;

		return *this;
	}



	inline double determinant()
	{
		return m00 * m11 - m01 * m10;
	}

	// BUGBUG - this should be optional
	Matrix2D inverse()
	{
		double d = determinant();

		// if (d == 0)
		// return nullptr;

		auto t00 = m11;
		auto t01 = m01;
		auto t10 = m10;
		auto t11 = m00;

		t00 = t00 / d;
		t01 = t01 / d;
		t10 = t10 / d;
		t11 = t11 / d;

		auto t20 = -(m20 * t00 + m21 * t10);
		auto t21 = -(m20 * t01 + m21 * t11);

		Matrix2D m1{ t00,t01,t10,t11,t20,t21 };

		return m1;
	}

	Matrix2D& rotate(const double angle, const double cx, const double cy)
	{
		Matrix2D r = Matrix2D::createRotation(angle, cx, cy);
		preMultiplyBy(r);

		return *this;
	}

	Matrix2D& scale(const double sx, const double sy)
	{
		m00 = m00 * sx;
		m01 = m01 * sx;
		m10 = m10 * sy;
		m11 = m11 * sy;

		return *this;
	}

	Matrix2D& translate(const double tx, const double ty)
	{
		double x1 = 0;
		double y1 = 0;
		transformPoint(tx, ty, x1, y1);
		m20 = m20 + x1;
		m21 = m21 + y1;

		return *this;
	}

	//
	// Apply the matrix to transforming points
	// transformPoint
	// transform an incoming point by the transformation matrix
	//
	void transformPoint(const double x, const double y, double& x1, double& y1)
	{
		x1 = m00 * x + m10 * y + m20;
		y1 = m01 * x + m11 * y + m21;
	}

	void dtransform(const double x, const double y, double& x1, double& y1)
	{
		x1 = m00 * x + m10 * y;
		y1 = m01 * x + m11 * y;
	}

	void scalePoint(const double x, const double y, double& x1, double& y1)
	{
		auto m1 = Matrix2D::createScaling(m00, m11);
		m1.transformPoint(x, y, x1, y1);
	}
};
