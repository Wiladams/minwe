#pragma once

#include "apidefs.h"


#include <functional>
#include <vector>


// A templatized parametric function that takes a single double
// value, and returns a value of the templatized type.
// The value of 'u' typically ranges from 0 to 1, but it
// can be anything
template <typename T>
using PFunc1 = std::function< T (const double u) >;

template <typename T>
using PFunc2 = std::function< T(const double u, const T & value) >;

template <typename T>
using PFunc2Ref = std::function< T &(const double u, T& value) >;


using PFunc1Double = PFunc1<double>;

/*
* A generalized structure for holding linear gradients.
	The general idea is to add 'stops' at various intervals
	Then, when you ask for a piece of data between offsets,
	it will do the interpolation to return that value.
*/

//
// InterpolatorStop
// Allows the user to specify a keypoint along the interpolation
template <typename T>
struct InterpolatorStop
{
	double offset;
	T data;

	InterpolatorStop() noexcept = default;
	InterpolatorStop(const InterpolatorStop<T>& other) noexcept = default;

	InterpolatorStop(const double off, const T& value) noexcept
		:offset(off),
		data(value) {}

	INLINE bool equals(const InterpolatorStop<T>& other) const noexcept
	{
		return (offset == other.offset) && (data == other.data);
	}

	INLINE bool operator==(const InterpolatorStop<T>& other) const noexcept { return  equals(other); }
	INLINE bool operator!=(const InterpolatorStop<T>& other) const noexcept { return !equals(other); }
};




template <typename T>
struct Interpolator
{
	std::vector<T> stops;

	// Find the closest stop to the specified value
	// we want the lowest index
	INLINE size_t searchClosestLast(const double& value) noexcept
	{
		size_t size = stops.size();
		if (stops.size() < 1)
			return 0;

		const T* base = stops;
		while (size_t half = size / 2u) {
			const T* middle = base + half;
			size -= half;
			if (middle[0].offset <= value)
				base = middle;
		}

		return size_t(base - stops);
	}

public:
	INLINE bool addStop(const double offset, const T& value) {

	}
};