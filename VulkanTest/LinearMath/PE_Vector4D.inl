#pragma once

#include "PE_Vector4D.h"

#ifndef __LINEAR_MATH_VECTOR4D_INL__
#define __LINEAR_MATH_VECTOR4D_INL__

template<typename T>
inline void Vector4D__<T>::operator=(const Vector4D__<T>& s)
{
	memcpy(&x, &s, sizeof(Vector4D__));
}

template<typename T>
inline bool Vector4D__<T>::operator==(const Vector4D__<T>& s) const
{
	return (x == s[0] && y == s[1] && z == s[2] && w == s[3]);
}

template<typename T>
inline bool Vector4D__<T>::operator!=(const Vector4D__<T>& s) const
{
	return !(*this == s);
}

template<typename T>
inline const T& Vector4D__<T>::operator[](size_t index) const
{
	return ((const T*)this)[index];
}

template<typename T>
inline T& Vector4D__<T>::operator[](size_t index)
{
	switch (index)
	{
	case 0:
		return this->x;
	case 1:
		return this->y;
	case 2:
		return this->z;
	case 3:
		return this->w;
	}
}


template<typename T>
inline const Vector4D__<T> Vector4D__<T>::operator+(const Vector4D__& s) const
{
	return Vector4D__(x + s[0], y + s[1], z + s[2], w + s[3]);
}

template<typename T>
inline Void Vector4D__<T>::operator+=(const Vector4D__<T>& s)
{
	*this = *this + s;
}

template<typename T>
inline const Vector4D__<T> Vector4D__<T>::operator+ () const
{
	return *this;
}


template<typename T>
inline const Vector4D__<T> Vector4D__<T>::operator-(const Vector4D__<T>& s) const
{
	return Vector4D__<T>(x - s[0], y - s[1], z - s[2], w - s[3]);
}

template<typename T>
inline Void Vector4D__<T>::operator-=(const Vector4D__<T>& s)
{
	*this = *this - s;
}

template<typename T>
inline const Vector4D__<T> Vector4D__<T>::operator-() const
{
	return Vector4D__<T>(-x, -y, -z, -w);
}


template<typename T>
inline const Vector4D__<T> Vector4D__<T>::operator*(const T& scalar) const
{
	return Vector4D__<T>(scalar * x, scalar * y, scalar * z, scalar * w);
}

template<typename T>
inline Void Vector4D__<T>::operator*=(const T& scalar)
{
	*this = *this * scalar;
}

//Dot product...

template<typename T>
inline const T Vector4D__<T>::operator*(const Vector4D__<T>& s) const
{
	return x * s[0] + y * s[1] + z * s[2] + w * s[3];
}

template<typename T>
inline const T Vector4D__<T>::dot(const Vector4D__<T>& s) const
{
	return *this * *this;
}

//Cross product...

template<typename T>
inline const Vector4D__<T>  Vector4D__<T>::operator^(const Vector4D__<T>& s) const
{
	Vector4D__<T> out;

	out[0] = y * s[2] - z * s[1];
	out[1] = z * s[0] - x * s[2];
	out[2] = x * s[1] - y * s[0];
	out[3] = s[3];

	return out;
}

//More...

template<typename T>
inline const double Vector4D__<T>::lenght() const
{
	return sqrt(*this * *this);//If sqrt define on this field
}

#endif