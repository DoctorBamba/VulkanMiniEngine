#pragma once

#include "PE_Types.h"

#ifndef __LINEAR_MATH_VECTOR4D_HEADER__
#define __LINEAR_MATH_VECTOR4D_HEADER__

template<typename T> class Vector4D__
{
	static const size_t dim = 4;
	private:
		//Data...
		T	x;
		T	y;
		T	z;
		T	w;
	public:
		Vector4D__() { this->x = T(); this->y = T(); this->z = T(); this->w = T(); }
		Vector4D__(const Vector4D__& vector) { *this = vector; }
		Vector4D__(const T& x, const T& y, const T& z, const T& w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

		//Operators...

		void operator=(const Vector4D__<T>& s);

		bool operator==(const Vector4D__<T>& s) const;
		bool operator!=(const Vector4D__<T>& s) const;

		const T& operator[](size_t index) const;
		T& operator[](size_t index);

		const Vector4D__ operator+(const Vector4D__& s) const;
		Void operator+=(const Vector4D__<T>& s);
		const Vector4D__<T> operator+ () const;

		const Vector4D__<T> operator-(const Vector4D__<T>& s) const;
		Void operator-=(const Vector4D__<T>& s);
		const Vector4D__<T> operator- () const;

		const Vector4D__<T> operator*(const T& scalar) const;
		friend const Vector4D__<T> operator*(const T scalar, const Vector4D__<T>& vector) { return vector * scalar; }
		Void operator*=(const T& scalar);

		//Dot product...

		const T operator*(const Vector4D__<T>& s) const;
		const T dot(const Vector4D__<T>& s) const;

		//Cross product...

		const Vector4D__<T> operator^(const Vector4D__<T>& s) const;

		//More...

		const double lenght() const;
};

//Difualt vector4D types...

typedef Vector4D__<Float> Vector4D;
typedef Vector4D__<Double> DVector4D;



#endif