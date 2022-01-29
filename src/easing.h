#pragma once

#include "maths.hpp"

// Reference
// https://easings.net/
//
namespace easing 
{
	// u - parametric value between [0..1]

	// straight linear interpolation between two values
	// This is a LERP.  You can combine this
	// with one of the easing functions
	template <typename T>
	INLINE T interpolator(double t, const T& a, const T& b)
	{
		return (T)(a * (1 - t) + b * t);
	}

	INLINE double easeLinear(double x)
	{
		return x;
	}

	// Back
	INLINE double backIn(double x)
	{
		const double c1 = 1.70158;
		const double c3 = c1 + 1;

		return c3 * x * x * x - c1 * x * x;
	}

	INLINE double backOut(double x)
	{
		const double c1 = 1.70158;
		const double c3 = c1 + 1;

		return 1 + c3 * std::pow(x - 1, 3) + c1 * std::pow(x - 1, 2);
	}

	INLINE double backInOut(double x)
	{
		const double c1 = 1.70158;
		const double c2 = c1 * 1.525;

		if (x < 0.5) {
			return (std::pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2;
		}

		return (std::pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
	}

	// Bounce
	INLINE double bounceOut(double x)
	{
		constexpr double n1 = 7.5625;
		constexpr double d1 = 2.75;


		if (x < (1 / d1)) {
			return n1 * x * x;
		}
		else if (x < (2 / d1)) {
			return n1 * (x -= (1.5 / d1)) * x + 0.75;
		}
		else if (x < (2.5 / d1)) {
			return n1 * (x -= 2.25 / d1) * x + 0.9275;
		}
		else {
			return n1 * (x -= 2.625 / d1) * x + 0.984375;
		}
	}
	
	INLINE double bounceIn(double x)
	{
		return 1 - bounceOut(1 - x);
	}

	INLINE double bounceInOut(double x)
	{
		if (x < 0.5) {
			return (1 - bounceOut(1 - 2 * x)) / 2;
		}

		return (1 + bounceOut(2 * x - 1)) / 2;
	}

	// Circle
	INLINE double circIn(double x)
	{
		return 1 - std::sqrt(1 - std::pow(x, 2));
	}

	INLINE double circOut(double x) {
		return std::sqrt(1 - std::pow(x - 1, 2));
	}

	INLINE double circInOut(double x)
	{
		if (x < 0.5) {
			return (1 - std::sqrt(1 - std::pow(2 * x, 2))) / 2;
		}

		return (std::sqrt(1 - std::pow(-2*x+2, 2))+1) / 2;
	}

	// Cubic
	INLINE double cubicIn(double x)
	{
		return x * x * x;
	}

	INLINE double cubicOut(double x)
	{
		return 1 - std::pow(1 - x, 3);
	}
	
	INLINE double cubicInOut(double x)
	{
		return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
	}

	// Elastic
	INLINE double elasticIn(double x)
	{
		const double c4 = (2 * maths::Pi) / 3;

		if (x == 0)
			return 0;
		else if (x == 1)
			return 1;


		return -pow(2, 10 * x - 10) * sin((x * 10 - 10.75) * c4);
	}




}
