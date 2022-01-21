#pragma once

#include "apidefs.h"
#include "vec.h"

struct mat2
{
	float e[4];

	mat2(float pm11, float pm12, float pm21, float pm22)
	{
		e[0] = pm11;
		e[1] = pm12;
		e[2] = pm21;
		e[3] = pm22;
	}

	// getting individual values
	INLINE constexpr float m11() const noexcept { return e[0]; }
	INLINE constexpr float m12() const noexcept { return e[1]; }
	INLINE constexpr float m21() const noexcept { return e[2]; }
	INLINE constexpr float m22() const noexcept { return e[3]; }

	// Setting individual values
	INLINE void setm11(const float value) noexcept { e[0] = value; }
	INLINE void setm12(const float value) noexcept { e[1] = value; }
	INLINE void setm21(const float value) noexcept { e[2] = value; }
	INLINE void setm22(const float value) noexcept { e[3] = value; }

	// index operators
	INLINE constexpr float operator[](int i) const { return e[i]; } // can't change the value
	INLINE float& operator[](int i) { return e[i]; }      // allows you to change the value

	INLINE float determinant() const {return (m11() * m22()) - (m12() * m21());}
};

struct mat3
{
	float e[9];

	mat3(float pm11, float pm12, float pm13, float pm21, float pm22, float pm23, float pm31, float pm32, float pm33)
	{
		e[0] = pm11;
		e[1] = pm12;
		e[2] = pm13;
		e[3] = pm21;
		e[4] = pm22;
		e[5] = pm23;
		e[6] = pm31;
		e[7] = pm32;
		e[8] = pm33;
	}

	mat3(const mat3& other) = default;

	//mat3(float [] values)
	//	:e{ values } {}

	// getting individual values
	INLINE constexpr float m11() const noexcept { return e[0]; }
	INLINE constexpr float m12() const noexcept { return e[1]; }
	INLINE constexpr float m13() const noexcept { return e[1]; }
	INLINE constexpr float m21() const noexcept { return e[3]; }
	INLINE constexpr float m22() const noexcept { return e[4]; }
	INLINE constexpr float m23() const noexcept { return e[5]; }
	INLINE constexpr float m31() const noexcept { return e[6]; }
	INLINE constexpr float m32() const noexcept { return e[7]; }
	INLINE constexpr float m33() const noexcept { return e[8]; }

	// Setting individual values
	INLINE void setm11(const float value) noexcept { e[0] = value; }
	INLINE void setm12(const float value) noexcept { e[1] = value; }
	INLINE void setm13(const float value) noexcept { e[2] = value; }
	INLINE void setm21(const float value) noexcept { e[3] = value; }
	INLINE void setm22(const float value) noexcept { e[4] = value; }
	INLINE void setm23(const float value) noexcept { e[5] = value; }
	INLINE void setm31(const float value) noexcept { e[6] = value; }
	INLINE void setm32(const float value) noexcept { e[7] = value; }
	INLINE void setm33(const float value) noexcept { e[8] = value; }

	// index operators
	INLINE constexpr float operator[](int i) const { return e[i]; } // can't change the value
	INLINE float& operator[](int i) { return e[i]; }      // allows you to change the value

	INLINE float determinant() const { return (m11() * m22()) - (m12() * m21()); }
};
