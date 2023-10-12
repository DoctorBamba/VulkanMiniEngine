#pragma once

#include "PE_Types.h"

#ifndef __LINEAR_MATH_VECTOR3D_HEADER__
#define __LINEAR_MATH_VECTOR3D_HEADER__

/***********************Vector3D**************************/

template<typename T> class Vector3D__
{
	static const size_t dim = 3;
	public:
		//Data...
		T	x;
		T	y;
		T	z;
	public:
		Vector3D__() { this->x = T(); this->y = T(); this->z = T(); }
		Vector3D__(const Vector3D__& vector) { *this = vector; }
		Vector3D__(const T& x, const T& y, const T& z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		//Operators...

		bool operator==(const Vector3D__<T>& s) const;
		bool operator!=(const Vector3D__<T>& s) const;

		const T& operator[](size_t index) const;
		T& operator[](size_t index);

		const Vector3D__ operator+(const Vector3D__& s) const;
		Void operator+=(const Vector3D__<T>& s);
		const Vector3D__<T> operator+ () const;

		const Vector3D__<T> operator-(const Vector3D__<T>& s) const;
		Void operator-=(const Vector3D__<T>& s);
		const Vector3D__<T> operator- () const;

		const Vector3D__<T> operator*(const T& scalar) const;
		friend const Vector3D__<T> operator*(const T scalar, const Vector3D__<T>& vector) { return vector * scalar; }
		Void operator*=(const T& scalar);

		//Dot product...

		const T operator*(const Vector3D__<T>& s) const;
		const T dot(const Vector3D__<T>& s) const;

		//Cross product...

		const Vector3D__<T> operator^(const Vector3D__<T>& s) const;

		//More...

		const double lenght() const;

		public:
			static Vector3D__<T> const Zero;
};

template<typename T> inline Vector3D__<T> const Vector3D__<T>::Zero = Vector3D__<T>(T(), T(), T());

//Difualt vector3D types...

typedef Vector3D__<Float> Vector3D;
typedef Vector3D__<Double> DVector3D;


//Cross product...

template<typename T>
inline const Vector3D__<T> Normalize(const Vector3D__<T>& v)
{
	return v * (T(1.0) / v.lenght());
}

template<typename T>
inline const Vector3D__<T> Cross(const Vector3D__<T>& f, const Vector3D__<T>& s)
{
	Vector3D__<T> result;

	result[0] = f[1] * s[2] - f[2] * s[1];
	result[1] = f[2] * s[0] - f[0] * s[2];
	result[2] = f[0] * s[1] - f[1] * s[0];

	return result;
}

template<typename T>
inline const T Dot(const Vector3D__<T>& f, const Vector3D__<T>& s)
{
	return f[0] * s[0] + f[1] * s[1] + f[2] * s[2];
}

template<typename T>
inline const Vector3D__<T> Reflect(const Vector3D__<T>& v, const Vector3D__<T>& n)
{
	return v - (2.0 * Dot(v, n) / Dot(n, n)) * n;
}


#endif