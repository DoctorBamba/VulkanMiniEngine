#pragma once

#include "PE_Types.h"

#ifndef __LINEAR_MATH_VECTOR2D_HEADER__
#define __LINEAR_MATH_VECTOR2D_HEADER__

/***********************Vector2D**************************/

template<typename T> class Vector2D__
{
	static const size_t dim = 2;
	private:
		//Data...
		T	x;
		T	y;
	public:
		Vector2D__() { this->x = T(); this->y = T(); }
		Vector2D__(const T& x, const T& y)
		{
			this->x = x;
			this->y = y;
		}

		//Operators...

		const bool operator==(const Vector2D__& s) const;
		const bool operator!=(const Vector2D__& s) const;

		const T& operator[](size_t index) const;
		T& operator[](size_t index);

		const Vector2D__ operator+(const Vector2D__& s) const;
		Vector2D__<T>& operator+=(const Vector2D__<T>& s);
		const Vector2D__<T> operator+ () const;

		const Vector2D__<T> operator-(const Vector2D__<T>& s) const;
		Vector2D__<T>& operator-=(const Vector2D__<T>& s);
		const Vector2D__<T> operator- () const;

		const Vector2D__<T> operator*(const T& scalar) const;
		friend const Vector2D__<T> operator*(const T scalar, const Vector2D__<T>& vector) { return vector * scalar; }
		Vector2D__<T>& operator*=(const T& scalar);

		//Dot product...

		const T operator*(const Vector2D__<T>& s) const;
		const T dot(const Vector2D__<T>& s) const;

		//More...

		const double lenght() const;
};

//Difualt vector2D types...

typedef Vector2D__<Float>		Vector2D;
typedef Vector2D__<Double>	DVector2D;
typedef Vector2D__<Int16>	Coordinates;

#endif