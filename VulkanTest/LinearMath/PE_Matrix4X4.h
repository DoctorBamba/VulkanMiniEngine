#pragma once

#include "PE_Vector4D.inl"

#ifndef __LINEAR_MATH_MATRIX4X4_HEADER__
#define __LINEAR_MATH_MATRIX4X4_HEADER__

/***********************Matrix4D**************************/

template<typename T> class Matrix4D__
{
	private:
		//Data...
		T	_00, _01, _02, _03;
		T	_10, _11, _12, _13;
		T	_20, _21, _22, _23;
		T	_30, _31, _32, _33;

	public:

		Matrix4D__() {
			_00 = T(); _01 = T(); _02 = T(); _03 = T();
			_10 = T(); _11 = T(); _12 = T(); _13 = T();
			_20 = T(); _21 = T(); _22 = T(); _23 = T();
			_30 = T(); _31 = T(); _32 = T(); _33 = T();
		}

		Matrix4D__(const Matrix4D__& matrix) { *this = matrix; }

		Matrix4D__(const T value) {
			_00 = value;	_01 = T();		_02 = T();		_03 = T();
			_10 = T();		_11 = value;	 _12 = T();		_13 = T();
			_20 = T();		_21 = T();		_22 = value;	_23 = T();
			_30 = T();		_31 = T();		 _32 = T();		_33 = value;
		}

		Matrix4D__(const T& i_00, const T& i_01, const T& i_02, const T& i_03,
			const T& i_10, const T& i_11, const T& i_12, const T& i_13,
			const T& i_20, const T& i_21, const T& i_22, const T& i_23,
			const T& i_30, const T& i_31, const T& i_32, const T& i_33)
		{
			this->_00 = i_00; this->_01 = i_01; this->_02 = i_02; this->_03 = i_03;
			this->_10 = i_10; this->_11 = i_11; this->_12 = i_12; this->_13 = i_13;
			this->_20 = i_20; this->_21 = i_21; this->_22 = i_22; this->_23 = i_23;
			this->_30 = i_30; this->_31 = i_31; this->_32 = i_32; this->_33 = i_33;
		}

		Matrix4D__(T* values)
		{
			memcpy(this, values, sizeof(T) * 16);
		}

		//Operators...
		void operator=(const Matrix4D__<T>& s);

		bool operator==(const Matrix4D__<T>& s) const;
		bool operator!=(const Matrix4D__<T>& s) const;

		const Vector4D__<T>& operator[](size_t index) const;
		Vector4D__<T>& operator[](size_t index);

		const Matrix4D__ operator+(const Matrix4D__& s) const;
		Void operator+=(const Matrix4D__<T>& s);
		const Matrix4D__<T> operator+ () const;

		const Matrix4D__<T> operator-(const Matrix4D__<T>& s) const;
		Void operator-=(const Matrix4D__<T>& s);
		const Matrix4D__<T> operator- () const;

		const Matrix4D__<T> operator*(const T& scalar) const;
		friend const Matrix4D__<T> operator*(const T scalar, const Matrix4D__<T>& matrix) { return matrix * scalar; }

		Matrix4D__<T>& operator*=(const T& scalar);

		const Matrix4D__<T> transpose() const;

		//Matrixs multiples..

		const Matrix4D__ operator*(const Matrix4D__& s) const;
		const Vector4D__<T> operator*(const Vector4D__<T>& s) const;
		friend const Vector4D__<T> operator*(const Vector4D__<T>& vector, const Matrix4D__<T>& matrix) { return matrix.transpose() * vector; }
};

//Difualt matrix4X4 types...

typedef Matrix4D__<Float> Matrix4D;
typedef Matrix4D__<Double> DMatrix4D;

template<typename T> 
const void Transpose(const Matrix4D__<T>& matrix, Matrix4D__<T>* out)
{
	const T* data_pointer = (const T*)(&matrix);
	T* data_out = (T*)out;

	data_out[0] = data_pointer[0];
	data_out[1] = data_pointer[4];
	data_out[2] = data_pointer[8];
	data_out[3] = data_pointer[12];
	data_out[4] = data_pointer[1];
	data_out[5] = data_pointer[5];
	data_out[6] = data_pointer[9];
	data_out[7] = data_pointer[13];
	data_out[8] = data_pointer[2];
	data_out[9] = data_pointer[6];
	data_out[10] = data_pointer[10];
	data_out[11] = data_pointer[14];
	data_out[12] = data_pointer[3];
	data_out[13] = data_pointer[7];
	data_out[14] = data_pointer[11];
	data_out[15] = data_pointer[15];

	return;
}

template<typename T>
const void Multiply(const Matrix4D__<T>& m1, const Matrix4D__<T>& m2, Matrix4D__<T>* out)
{
	const T* data1 = (const T*)(&m1);
	const T* data2 = (const T*)(&m2);
	T* data_out = (T*)out;

	data_out[0] = data1[0] * data2[0] + data1[1] * data2[4] + data1[2] * data2[8] + data1[3] * data2[12];
	data_out[1] = data1[0] * data2[1] + data1[1] * data2[5] + data1[2] * data2[9] + data1[3] * data2[13];
	data_out[2] = data1[0] * data2[2] + data1[1] * data2[6] + data1[2] * data2[10] + data1[3] * data2[14];
	data_out[3] = data1[0] * data2[3] + data1[1] * data2[7] + data1[2] * data2[11] + data1[3] * data2[15];

	data_out[4] = data1[4] * data2[0] + data1[5] * data2[4] + data1[6] * data2[8] + data1[7] * data2[12];
	data_out[5] = data1[4] * data2[1] + data1[5] * data2[5] + data1[6] * data2[9] + data1[7] * data2[13];
	data_out[6] = data1[4] * data2[2] + data1[5] * data2[6] + data1[6] * data2[10] + data1[7] * data2[14];
	data_out[7] = data1[4] * data2[3] + data1[5] * data2[7] + data1[6] * data2[11] + data1[7] * data2[15];

	data_out[8] = data1[8] * data2[0] + data1[9] * data2[4] + data1[10] * data2[8] + data1[11] * data2[12];
	data_out[9] = data1[8] * data2[1] + data1[9] * data2[5] + data1[10] * data2[9] + data1[11] * data2[13];
	data_out[10] = data1[8] * data2[2] + data1[9] * data2[6] + data1[10] * data2[10] + data1[11] * data2[14];
	data_out[11] = data1[8] * data2[3] + data1[9] * data2[7] + data1[10] * data2[11] + data1[11] * data2[15];

	data_out[12] = data1[12] * data2[0] + data1[13] * data2[4] + data1[14] * data2[8] + data1[15] * data2[12];
	data_out[13] = data1[12] * data2[1] + data1[13] * data2[5] + data1[14] * data2[9] + data1[15] * data2[13];
	data_out[14] = data1[12] * data2[2] + data1[13] * data2[6] + data1[14] * data2[10] + data1[15] * data2[14];
	data_out[15] = data1[12] * data2[3] + data1[13] * data2[7] + data1[14] * data2[11] + data1[15] * data2[15];
	
	return;
}

template<typename T>
const void Multiply(const Matrix4D__<T>& m, const Vector4D__<T>& v, Vector4D__<T>* out)
{
	const T* data_m = (const T*)(&m);
	const T* data_v = (const T*)(&v);
	T* data_out = (T*)out;

	data_out[0] = data_m[0] * data_v[0] + data_m[1] * data_v[1] + data_m[2] * data_v[2] + data_m[3] * data_v[3];
	data_out[1] = data_m[4] * data_v[0] + data_m[5] * data_v[1] + data_m[6] * data_v[2] + data_m[7] * data_v[3];
	data_out[2] = data_m[8] * data_v[0] + data_m[9] * data_v[1] + data_m[10] * data_v[2] + data_m[11] * data_v[3];
	data_out[3] = data_m[12] * data_v[0] + data_m[13] * data_v[1] + data_m[14] * data_v[2] + data_m[15] * data_v[3];
}

template<typename T>
const void Multiply(const Vector4D__<T>& v, const Matrix4D__<T>& m, Vector4D__<T>* out)
{
	const T* data_m = (const T*)(&m);
	const T* data_v = (const T*)(&v);
	T* data_out = (T*)out;

	data_out[0] = data_v[0] * data_m[0] + data_v[1] * data_m[4] + data_v[2] * data_m[8] + data_v[3] * data_m[12];
	data_out[1] = data_v[0] * data_m[1] + data_v[1] * data_m[5] + data_v[2] * data_m[9] + data_v[3] * data_m[13];
	data_out[2] = data_v[0] * data_m[2] + data_v[1] * data_m[6] + data_v[2] * data_m[10] + data_v[3] * data_m[14];
	data_out[3] = data_v[0] * data_m[3] + data_v[1] * data_m[7] + data_v[2] * data_m[11] + data_v[3] * data_m[15];
	
	return;
}


template<typename T>
const void Interpolate(const T& f, const T& s, Float t)
{
	return (1.0f - t) * f + t * s;
}

#endif