#pragma once

#include "PE_Vector3D.h"

#ifndef __LINEAR_MATH_VECTOR3D_INL__
#define __LINEAR_MATH_VECTOR3D_INL__

template<typename T>
inline bool Vector3D__<T>::operator==(const Vector3D__<T>& s) const
{
	return (x == s[0] && y == s[1] && z == s[2]);
}

template<typename T>
inline bool Vector3D__<T>::operator!=(const Vector3D__<T>& s) const
{
	return !(*this == s);
}

template<typename T>
inline const T& Vector3D__<T>::operator[](size_t index) const
{
	return ((const T*)(this))[index];
}

template<typename T>
inline T& Vector3D__<T>::operator[](size_t index)
{
	switch (index)
	{
	case 0:
		return this->x;
	case 1:
		return this->y;
	case 2:
		return this->z;
	}
}


template<typename T>
inline const Vector3D__<T> Vector3D__<T>::operator+(const Vector3D__& s) const
{
	return Vector3D__(x + s[0], y + s[1], z + s[2]);
}

template<typename T>
inline Void Vector3D__<T>::operator+=(const Vector3D__<T>& s)
{
	*this = *this + s;
}

template<typename T>
inline const Vector3D__<T> Vector3D__<T>::operator+ () const
{
	return *this;
}


template<typename T>
inline const Vector3D__<T> Vector3D__<T>::operator-(const Vector3D__<T>& s) const
{
	return Vector3D__<T>(x - s[0], y - s[1], z - s[2]);
}

template<typename T>
inline Void Vector3D__<T>::operator-=(const Vector3D__<T>& s)
{
	*this = *this - s;
}

template<typename T>
inline const Vector3D__<T> Vector3D__<T>::operator-() const
{
	return Vector3D__<T>(-x, -y, -z);
}


template<typename T>
inline const Vector3D__<T> Vector3D__<T>::operator*(const T& scalar) const
{
	return Vector3D__<T>(scalar * x, scalar * y, scalar * z);
}

template<typename T>
inline Void Vector3D__<T>::operator*=(const T& scalar)
{
	*this = *this * scalar;
}

//Dot product...

template<typename T>
inline const T Vector3D__<T>::operator*(const Vector3D__<T>& s) const
{
	return x * s[0] + y * s[1] + z * s[2];
}

template<typename T>
inline const T Vector3D__<T>::dot(const Vector3D__<T>& s) const
{
	return *this * s;
}

//Cross product...

template<typename T>
inline const Vector3D__<T>  Vector3D__<T>::operator^(const Vector3D__<T>& s) const
{
	Vector3D__<T> out;

	out[0] = y * s[2] - z * s[1];
	out[1] = z * s[0] - x * s[2];
	out[2] = x * s[1] - y * s[0];

	return out;
}

//More...

template<typename T>
inline const double Vector3D__<T>::lenght() const
{
	return sqrt(*this * *this);//If sqrt define on this field
}


#endif