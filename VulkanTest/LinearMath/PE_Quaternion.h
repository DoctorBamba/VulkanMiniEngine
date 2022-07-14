#pragma once

#include "PE_Vector3D.h"

#ifndef __LINEAR_MATH_QUATERNION_HEADER__
#define __LINEAR_MATH_QUATERNION_HEADER__

/********************Quaternions...***************************/

template<typename T> class Quaternion__
{
	private:
		//Data...
		T	w;
		T	nx;
		T	ny;
		T	nz;
	public:
		Quaternion__() { this->w = T(); this->nx = T(); this->ny = T(); this->nz = T(); }
		Quaternion__(const Quaternion__& quat) { *this = quat; }
		Quaternion__(const T& w, const T& nx, const T& ny, const T& nz)
		{
			this->w = w;
			this->nx = nx;
			this->ny = ny;
			this->nz = nz;
		}
		Quaternion__(const T angle, const Vector3D__<T>& axis)
		{
			T c = cos(angle * 0.5f);
			T s = sin(angle * 0.5f);

			this->w = c;
			this->nx = s * axis[0];
			this->ny = s * axis[1];
			this->nz = s * axis[2];
		}

		//Operators...

		bool operator==(const Quaternion__<T>& s) const;
		bool operator!=(const Quaternion__<T>& s) const;

		const T& operator[](size_t index) const;
		T& operator[](size_t index);

		const Quaternion__ operator+(const Quaternion__& s) const;
		Quaternion__<T>& operator+=(const Quaternion__<T>& s);
		const Quaternion__<T> operator+ () const;

		const Quaternion__<T> operator-(const Quaternion__<T>& s) const;
		Quaternion__<T>& operator-=(const Quaternion__<T>& s);
		const Quaternion__<T> operator- () const;

		const Quaternion__<T> operator*(const T& scalar) const;
		friend const Quaternion__<T> operator*(const T scalar, const Quaternion__<T>& qaud) { return qaud * scalar; }
		Quaternion__<T>& operator*=(const T& scalar);

		const Quaternion__ operator*(const Quaternion__& s) const;
		Quaternion__<T>& operator*=(const Quaternion__<T>& s);

		//More...

		const T norm() const;
};

//Difualt quaternion types...

typedef Quaternion__<Float> Quaternion;
typedef Quaternion__<Double> DQuaternion;


#endif