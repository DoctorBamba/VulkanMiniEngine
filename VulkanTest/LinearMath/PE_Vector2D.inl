#pragma once

#include "PE_Vector2D.h"

#ifndef __LINEAR_MATH_VECTOR2D_INL__
#define __LINEAR_MATH_VECTOR2D_INL__

template<typename T>
inline const bool Vector2D__<T>::operator==(const Vector2D__& s) const
{
	return (bool)(x == s[0] && y == s[1]);
}

template<typename T>
inline const  bool Vector2D__<T>::operator!=(const Vector2D__& s) const
{
	return !(*this == s);
}

template<typename T>
inline const T& Vector2D__<T>::operator[](size_t index) const
{
	return ((const T*)(this))[index];
}

template<typename T>
inline T& Vector2D__<T>::operator[](size_t index)
{
	if (index == 0)
		return x;
	else
		return y;
}


template<typename T>
inline const Vector2D__<T> Vector2D__<T>::operator+(const Vector2D__& s) const
{
	return Vector2D__(x + s[0], y + s[1]);
}

template<typename T>
inline Vector2D__<T>& Vector2D__<T>::operator+=(const Vector2D__<T>& s)
{
	return *this = *this + s;
}

template<typename T>
inline const Vector2D__<T> Vector2D__<T>::operator+ () const
{
	return *this;
}


template<typename T>
inline const Vector2D__<T> Vector2D__<T>::operator-(const Vector2D__<T>& s) const
{
	return Vector2D__<T>(x - s[0], y - s[1]);
}

template<typename T>
inline Vector2D__<T>& Vector2D__<T>::operator-=(const Vector2D__<T>& s) 
{
	return *this = *this - s;
}

template<typename T>
inline const Vector2D__<T> Vector2D__<T>::operator-() const
{
	return Vector2D__<T>(-x, -y);
}


template<typename T>
inline const Vector2D__<T> Vector2D__<T>::operator*(const T& scalar) const
{
	return Vector2D__<T>(scalar * x, scalar * y);
}

template<typename T>
inline Vector2D__<T>& Vector2D__<T>::operator*=(const T& scalar)
{
	return *this = *this * scalar;
}

//Dot product...

template<typename T>
inline const T Vector2D__<T>::operator*(const Vector2D__<T>& s) const
{
	return x * s[0] + y * s[1];
}

template<typename T>
inline const T Vector2D__<T>::dot(const Vector2D__<T>& s) const
{
	return *this * *this;
}

//More...

template<typename T>
inline const double Vector2D__<T>::lenght() const
{
	return sqrt(*this * *this);//If sqrt define on this field
}

#endif