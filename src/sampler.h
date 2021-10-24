#pragma once

// The ISample interface is meant to support a generic interface
// for generating color values based on parameters.
// Doing this makes it easy to create color values for different
// rendering situations, without limiting ourselves to bitmaps.
//
// You can create a sample to return a certain type using the 
// teamplate class Sample<T>.
//
// struct SolidColorSample2D : ISample2D<PixelRGBA>
//

// A 1 dimensional sampler
template <typename T>
struct ISample1D
{
	virtual T operator()(double u) const = 0;
};

// A 2 dimentional sampler
template <typename T>
struct ISample2D
{
	virtual T operator()(double u, double v) const = 0;
};

// A 3 dimensional sampler
template <typename T>
struct ISample3D
{
	virtual T operator()(double u, double v, double w) const = 0;
};
