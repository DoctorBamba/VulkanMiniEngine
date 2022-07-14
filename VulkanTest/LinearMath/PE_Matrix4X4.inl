#pragma once

#include "PE_Matrix4X4.h"

#ifndef __LINEAR_MATH_MATRIX4X4_INL__
#define __LINEAR_MATH_MATRIX4X4_INL__

template<typename T>
inline void Matrix4D__<T>::operator=(const Matrix4D__<T>& s)
{
	memcpy(&_00, &s, sizeof(Matrix4D__));
}

template<typename T>
inline bool Matrix4D__<T>::operator==(const Matrix4D__<T>& s) const
{
	for (Uint i = 0; i < 16; i++)
	{
		if ((T*)(&_00)[i] != (T*)(&s[0])[i])
			return false;
	}

	return true;
}

template<typename T>
inline bool Matrix4D__<T>::operator!=(const Matrix4D__<T>& s) const
{
	return !(*this == s);
}

template<typename T>
inline const Vector4D__<T>& Matrix4D__<T>::operator[](size_t index) const
{
	return ((const Vector4D__<T>*)(&_00))[index];
}

template<typename T>
inline Vector4D__<T>& Matrix4D__<T>::operator[](size_t index)
{
	return ((Vector4D__<T>*)(&_00))[index];
}


template<typename T>
inline const Matrix4D__<T> Matrix4D__<T>::operator+(const Matrix4D__& s) const
{
	return Matrix4D__(_00 + s[0][0], _01 + s[0][1], _02 + s[0][2], _03 + s[0][3],
						_10 + s[1][0], _11 + s[1][1], _12 + s[1][2], _13 + s[1][3],
						_20 + s[2][0], _21 + s[2][1], _22 + s[2][2], _23 + s[2][3],
						_30 + s[3][0], _31 + s[3][1], _32 + s[3][2], _33 + s[3][3]);
}

template<typename T>
inline Void Matrix4D__<T>::operator+=(const Matrix4D__<T>& s)
{
	*this = *this + s;
}

template<typename T>
inline const Matrix4D__<T> Matrix4D__<T>::operator+ () const
{
	return *this;
}


template<typename T>
inline const Matrix4D__<T> Matrix4D__<T>::operator-(const Matrix4D__<T>& s) const
{
	return Matrix4D__(_00 - s[0][0], _01 - s[0][1], _02 - s[0][2], _03 - s[0][3],
		_10 - s[1][0], _11 - s[1][1], _12 - s[1][2], _13 - s[1][3],
		_20 - s[2][0], _21 - s[2][1], _22 - s[2][2], _23 - s[2][3],
		_30 - s[3][0], _31 - s[3][1], _32 - s[3][2], _33 - s[3][3]);
}

template<typename T>
inline Void Matrix4D__<T>::operator-=(const Matrix4D__<T>& s)
{
	*this = *this - s;
}

template<typename T>
inline const Matrix4D__<T> Matrix4D__<T>::operator-() const
{
	return Matrix4D__<T>(-_00, -_01, -_02, -_03,
		-_10, -_11, -_12, -_13,
		-_20, -_21, -_22, -_23,
		-_30, -_31, -_32, -_33);
}


template<typename T>
inline const Matrix4D__<T> Matrix4D__<T>::operator*(const T& scalar) const
{
	return Matrix4D__<T>(scalar * _00, scalar * _01, scalar * _02, scalar * _03,
		scalar * _10, scalar * _11, scalar * _12, scalar * _13,
		scalar * _20, scalar * _21, scalar * _22, scalar * _23,
		scalar * _30, scalar * _31, scalar * _32, scalar * _33);
}

template<typename T>
inline Matrix4D__<T>& Matrix4D__<T>::operator*=(const T& scalar)
{
	return *this = *this * scalar;
}


//Matrix multiple

template<typename T>
inline const Matrix4D__<T> Matrix4D__<T>::operator*(const Matrix4D__& s) const//Matrix4-Matrix4 multiply
{
	const T* d = (T*)&s;
	return Matrix4D__(	_00 * d[0] + _01 * d[4] + _02 * d[8] + _03 * d[12],
							_00 * d[1] + _01 * d[5] + _02 * d[9] + _03 * d[13],
							_00 * d[2] + _01 * d[6] + _02 * d[10] + _03 * d[14],
							_00 * d[3] + _01 * d[7] + _02 * d[11] + _03 * d[15],

							_10 * d[0] + _11 * d[4] + _12 * d[8] + _13 * d[12],
							_10 * d[1] + _11 * d[5] + _12 * d[9] + _13 * d[13],
							_10 * d[2] + _11 * d[6] + _12 * d[10] + _13 * d[14],
							_10 * d[3] + _11 * d[7] + _12 * d[11] + _13 * d[15],

							_20 * d[0] + _21 * d[4] + _22 * d[8] + _23 * d[12],
							_20 * d[1] + _21 * d[5] + _22 * d[9] + _23 * d[13],
							_20 * d[2] + _21 * d[6] + _22 * d[10] + _23 * d[14],
							_20 * d[3] + _21 * d[7] + _22 * d[11] + _23 * d[15],

							_30 * d[0] + _31 * d[4] + _32 * d[8] + _33 * d[12],
							_30 * d[1] + _31 * d[5] + _32 * d[9] + _33 * d[13],
							_30 * d[2] + _31 * d[6] + _32 * d[10] + _33 * d[14],
							_30 * d[3] + _31 * d[7] + _32 * d[11] + _33 * d[15]);
}

template<typename T>
inline const Vector4D__<T> Matrix4D__<T>::operator*(const Vector4D__<T>& v) const//Matrix4-Vector4 multiply
{
	return Vector4D__<T>(_00 * v[0] + _01 * v[1] + _02 * v[2] + _03 * v[3],
		_10 * v[0] + _11 * v[1] + _12 * v[2] + _13 * v[3],
		_20 * v[0] + _21 * v[1] + _22 * v[2] + _23 * v[3],
		_30 * v[0] + _31 * v[1] + _32 * v[2] + _33 * v[3]);
}

template<typename T>
inline const Matrix4D__<T> Matrix4D__<T>::transpose() const
{
	return Matrix4D__<T>(_00, _10, _20, _30,
							_01, _11, _21, _31,
							_02, _12, _22, _32,
							_03, _13, _23, _33);
}

#endif