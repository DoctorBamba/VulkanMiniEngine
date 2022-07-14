#pragma once

#include "PE_Quaternion.h"

#ifndef __LINEAR_MATH_QUATERNION_INL__
#define __LINEAR_MATH_QUATERNION_INL__

template<typename T>
inline bool Quaternion__<T>::operator==(const Quaternion__<T>& s) const
{
	return (w == s[0] && nx == s[1] && ny == s[2] && nz == s[3]);
}

template<typename T>
inline bool Quaternion__<T>::operator!=(const Quaternion__<T>& s) const
{
	return !(*this == s);
}

template<typename T>
inline const T& Quaternion__<T>::operator[](size_t index) const
{
	if (index > 3)
		std::range_error("PE_Quaternion__::operator[] Error - index out of range");
	
	return static_cast<const T*>(&w)[index];
}

template<typename T>
inline T& Quaternion__<T>::operator[](size_t index)
{
	if (index > 3)
		std::range_error("PE_Quaternion__::operator[] Error - index out of range");

	return static_cast<T*>(&w)[index];
}


template<typename T>
inline const Quaternion__<T> Quaternion__<T>::operator+(const Quaternion__& s) const
{
	return Quaternion__(w + s[0], nx + s[1], ny + s[2], nz + s[3]);
}

template<typename T>
inline Quaternion__<T>& Quaternion__<T>::operator+=(const Quaternion__<T>& s)
{
	return *this = *this + s;
}

template<typename T>
inline const Quaternion__<T> Quaternion__<T>::operator+ () const
{
	return *this;
}


template<typename T>
inline const Quaternion__<T> Quaternion__<T>::operator-(const Quaternion__<T>& s) const
{
	return Quaternion__<T>(w - s[0], nx - s[1], ny - s[2], nz - s[3]);
}

template<typename T>
inline Quaternion__<T>& Quaternion__<T>::operator-=(const Quaternion__<T>& s)
{
	return *this = *this - s;
}

template<typename T>
inline const Quaternion__<T> Quaternion__<T>::operator-() const
{
	return Quaternion__<T>(-w, -nx, -ny, -nz);
}


template<typename T>
inline const Quaternion__<T> Quaternion__<T>::operator*(const T& scalar) const
{
	return Quaternion__<T>(scalar * w, scalar * nx, scalar * ny, scalar * nz);
}

template<typename T>
inline Quaternion__<T>& Quaternion__<T>::operator*=(const T& scalar)
{
	return *this = *this * scalar;
}

//Multiple...

template<typename T>
inline const Quaternion__<T> Quaternion__<T>::operator*(const Quaternion__& s) const
{
	return Quaternion__(	w * s[0] - nx * s[1] - ny * s[2] - nz * s[3],
							w * s[1] + nx * s[0] + ny * s[3] - nz * s[2],
							w * s[2] + ny * s[0] - nx * s[3] + nz * s[1],
							w * s[3] + nz * s[0] + nx * s[2] - ny * s[1]);
}

template<typename T>
inline Quaternion__<T>& Quaternion__<T>::operator*=(const Quaternion__<T>& s)
{
	return *this = *this * s;
}

template<typename T>
const T Quaternion__<T>::norm() const
{
	return w * w + nx * nx + ny * ny + nz * nz;
}

#endif