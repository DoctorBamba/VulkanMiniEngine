#pragma once

#include "PE_Matrix3X3.h"

#ifndef __LINEAR_MATH_MATRIX3X3_INL__
#define __LINEAR_MATH_MATRIX3X3_INL__

template<typename T>
inline void Matrix3D__<T>::operator=(const Matrix3D__<T>& s)
{
	memcpy(&_00, &s, sizeof(Matrix3D__));
}

template<typename T>
inline bool Matrix3D__<T>::operator==(const Matrix3D__<T>& s) const
{
	for (Uint i = 0; i < 9; i++)
	{
		if ((T*)(&_00)[i] != (T*)(&s[0])[i])
			return false;
	}

	return true;
}

template<typename T>
inline bool Matrix3D__<T>::operator!=(const Matrix3D__<T>& s) const
{
	return !(*this == s);
}

template<typename T>
inline const Vector3D__<T>& Matrix3D__<T>::operator[](size_t index) const
{
	return ((const Vector3D__<T>*)(&_00))[index];
}

template<typename T>
inline Vector3D__<T>& Matrix3D__<T>::operator[](size_t index)
{
	return ((Vector3D__<T>*)(&_00))[index];
}



template<typename T>
inline const Matrix3D__<T> Matrix3D__<T>::operator+(const Matrix3D__& s) const
{
	return Matrix3D__<T>(_00 + s[0][0], _01 + s[0][1], _02 + s[0][2],
		_10 + s[1][0], _11 + s[1][1], _12 + s[1][2],
		_20 + s[2][0], _21 + s[2][1], _22 + s[2][2]);
}

template<typename T>
inline void Matrix3D__<T>::operator+=(const Matrix3D__<T>& s)
{
	*this = *this + s;
}

template<typename T>
inline const Matrix3D__<T> Matrix3D__<T>::operator+ () const
{
	return *this;
}


template<typename T>
inline const Matrix3D__<T> Matrix3D__<T>::operator-(const Matrix3D__<T>& s) const
{
	return Matrix3D__<T>(_00 - s[0][0], _01 - s[0][1], _02 - s[0][2],
		_10 - s[1][0], _11 - s[1][1], _12 - s[1][2],
		_20 - s[2][0], _21 - s[2][1], _22 - s[2][2]);
}

template<typename T>
inline Matrix3D__<T>& Matrix3D__<T>::operator-=(const Matrix3D__<T>& s)
{
	return *this = *this - s;
}

template<typename T>
inline const Matrix3D__<T> Matrix3D__<T>::operator-() const
{
	return Matrix3D__<T>(-_00, -_01, -_02,
		-_10, -_11, -_12,
		-_20, -_21, -_22);
}


template<typename T>
inline const Matrix3D__<T> Matrix3D__<T>::operator*(const T& scalar) const
{
	return Matrix3D__<T>(scalar * _00, scalar * _01, scalar * _02,
		scalar * _10, scalar * _11, scalar * _12,
		scalar * _20, scalar * _21, scalar * _22);
}

template<typename T>
inline Matrix3D__<T>& Matrix3D__<T>::operator*=(const T& scalar)
{
	return *this = *this * scalar;
}


template<typename T>
inline const Matrix3D__<T> Matrix3D__<T>::transpose() const
{
	return Matrix3D__<T>(_00, _10, _20,
							_01, _11, _21,
							_02, _12, _22);
}


//Matrix multiple

template<typename T>
inline const Matrix3D__<T> Matrix3D__<T>::operator*(const Matrix3D__& s) const//Matrix4-Matrix4 multiply
{
	const T* d = (T*)&s;
	return Matrix3D__<T>(_00 * d[0] + _01 * d[3] + _02 * d[6],
		_00 * d[1] + _01 * d[4] + _02 * d[7],
		_00 * d[2] + _01 * d[5] + _02 * d[8],

		_10 * d[0] + _11 * d[3] + _12 * d[6],
		_10 * d[1] + _11 * d[4] + _12 * d[7],
		_10 * d[2] + _11 * d[5] + _12 * d[8],

		_20 * d[0] + _21 * d[3] + _22 * d[6],
		_20 * d[1] + _21 * d[4] + _22 * d[7],
		_20 * d[2] + _21 * d[5] + _22 * d[8]);
}


#endif