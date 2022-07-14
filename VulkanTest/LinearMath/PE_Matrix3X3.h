#pragma once

#include "PE_Vector3D.inl"

#ifndef __LINEAR_MATH_MATRIX3X3_HEADER__
#define __LINEAR_MATH_MATRIX3X3_HEADER__

/***********************Matrix3X3**************************/

template<typename T> class Matrix3D__
{
	private:
		//Data...
		T	_00, _01, _02;
		T	_10, _11, _12;
		T	_20, _21, _22;

	public:
		Matrix3D__() {
			_00 = T(); _01 = T(); _02 = T();
			_10 = T(); _11 = T(); _12 = T();
			_20 = T(); _21 = T(); _22 = T();
		}

		Matrix3D__(const Matrix3D__& matrix) { *this = matrix; }

		Matrix3D__(const T value) {
			_00 = value;	_01 = T();		_02 = T();
			_10 = T();		_11 = value;	_12 = T();
			_20 = T();		_21 = T();		_22 = value;
		}

		Matrix3D__(const T& i_00, const T& i_01, const T& i_02,
			const T& i_10, const T& i_11, const T& i_12,
			const T& i_20, const T& i_21, const T& i_22)
		{
			this->_00 = i_00; this->_01 = i_01; this->_02 = i_02;
			this->_10 = i_10; this->_11 = i_11; this->_12 = i_12;
			this->_20 = i_20; this->_21 = i_21; this->_22 = i_22;
		}

		//Operators...
		void operator=(const Matrix3D__<T>& s);

		bool operator==(const Matrix3D__<T>& s) const;
		bool operator!=(const Matrix3D__<T>& s) const;

		const Vector3D__<T>& operator[](size_t index) const;
		Vector3D__<T>& operator[](size_t index);

		const Matrix3D__ operator+(const Matrix3D__& s) const;
		void operator+=(const Matrix3D__<T>& s);
		const Matrix3D__<T> operator+ () const;

		const Matrix3D__<T> operator-(const Matrix3D__<T>& s) const;
		Matrix3D__<T>& operator-=(const Matrix3D__<T>& s);
		const Matrix3D__<T> operator- () const;

		const Matrix3D__<T> operator*(const T& scalar) const;
		friend const Matrix3D__<T> operator*(const T scalar, const Matrix3D__<T>& matrix) { return matrix * scalar; }

		Matrix3D__<T>& operator*=(const T& scalar);

		const Matrix3D__<T> transpose() const;

		//Matrixs multiples..

		const Matrix3D__ operator*(const Matrix3D__& s) const;
		
		template<typename field>
		friend const Vector3D__<field> operator*(const Matrix3D__<field>& m, const Vector3D__<field>& v);
		
		template<typename field>
		friend const Vector3D__<field> operator*(const Vector3D__<field>& v, const Matrix3D__<field>& m);
};

template<typename T> 
const void Transpose(const Matrix3D__<T>& matrix, Matrix3D__<T>* out)
{
	const T* data_pointer = (const T*)(&matrix);
	T* data_out = (T*)out;

	data_out[0] = data_pointer[0];
	data_out[1] = data_pointer[3];
	data_out[2] = data_pointer[6];
	data_out[3] = data_pointer[1];
	data_out[4] = data_pointer[4];
	data_out[5] = data_pointer[7];
	data_out[6] = data_pointer[2];
	data_out[7] = data_pointer[5];
	data_out[8] = data_pointer[8];

	return;
}

template<typename T>
const void Multiply(const Matrix3D__<T>& m1, const Matrix3D__<T>& m2, Matrix3D__<T>* out)
{
	const T* data1 = (const T*)(&m1);
	const T* data2 = (const T*)(&m2);
	T* data_out = (T*)out;

	out[0] = data1[0] * data2[0] + data1[1] * data2[3] + data1[2] * data2[6];
	out[1] = data1[0] * data2[1] + data1[1] * data2[4] + data1[2] * data2[7];
	out[2] = data1[0] * data2[2] + data1[1] * data2[5] + data1[2] * data2[8];
	out[3] = data1[3] * data2[0] + data1[4] * data2[3] + data1[5] * data2[6];
	out[4] = data1[3] * data2[1] + data1[4] * data2[4] + data1[5] * data2[7];
	out[5] = data1[3] * data2[2] + data1[4] * data2[5] + data1[5] * data2[8];
	out[6] = data1[6] * data2[0] + data1[7] * data2[3] + data1[8] * data2[6];
	out[7] = data1[6] * data2[1] + data1[7] * data2[4] + data1[8] * data2[7];
	out[8] = data1[6] * data2[2] + data1[7] * data2[5] + data1[8] * data2[8];

	return;
}

template<typename T>
const void Multiply(const Matrix3D__<T>& m, const Vector3D__<T>& v, Vector3D__<T>* out)
{
	const T* data_m = (const T*)(&m);
	const T* data_v = (const T*)(&v);
	T* data_out = (T*)out;

	data_out[0] = data_m[0] * data_v[0] + data_m[1] * data_v[1] + data_m[2] * data_v[2];
	data_out[1] = data_m[3] * data_v[0] + data_m[4] * data_v[1] + data_m[5] * data_v[2];
	data_out[2] = data_m[6] * data_v[0] + data_m[7] * data_v[1] + data_m[8] * data_v[2];
}

template<typename T>
const void Multiply(const Vector3D__<T>& v, const Matrix3D__<T>& m, Vector3D__<T>* out)
{
	const T* data_m = (const T*)(&m);
	const T* data_v = (const T*)(&v);
	T* data_out = (T*)out;

	data_out[0] = data_v[0] * data_m[0] + data_v[1] * data_m[3] + data_v[2] * data_m[6];
	data_out[1] = data_v[0] * data_m[1] + data_v[1] * data_m[4] + data_v[2] * data_m[7];
	data_out[2] = data_v[0] * data_m[2] + data_v[1] * data_m[5] + data_v[2] * data_m[8];
}



template<typename field>
inline const Vector3D__<field> operator*(const Matrix3D__<field>& m, const Vector3D__<field>& v)//Matrix3-Vector3 multiply
{
	Vector3D__<field> output;
	Multiply<field>(m, v, &output);
	return output;
}

template<typename field>
inline const Vector3D__<field> operator*(const Vector3D__<field>& v, const Matrix3D__<field>& m)//Vector3-Matrix3 multiply
{
	Vector3D__<field> output;
	Multiply<field>(v, m, &output);
	return output;
}

template<typename T>
const void Inverse(const Matrix3D__<T>& m, Matrix3D__<T>* out)
{
	const T* data_m = (const T*)(&m);
	T* data_out = (T*)out;

	const T a00 =  (data_m[4] * data_m[8] - data_m[5] * data_m[7]);
	const T a01 = -(data_m[3] * data_m[8] - data_m[5] * data_m[6]);
	const T a02 =  (data_m[3] * data_m[7] - data_m[4] * data_m[6]);

	const T a10 = -(data_m[1] * data_m[8] - data_m[2] * data_m[7]);
	const T a11 =  (data_m[0] * data_m[8] - data_m[2] * data_m[6]);
	const T a12 = -(data_m[0] * data_m[7] - data_m[1] * data_m[6]);

	const T a20 =  (data_m[1] * data_m[5] - data_m[2] * data_m[4]);
	const T a21 = -(data_m[0] * data_m[5] - data_m[2] * data_m[3]);
	const T a22 =  (data_m[0] * data_m[4] - data_m[1] * data_m[3]);

	const T det = data_m[0] * a00 + data_m[1] * a01 + data_m[2] * a02;

	data_out[0] = a00 / det; data_out[1] = a01 / det; data_out[2] = a02 / det;
	data_out[3] = a10 / det; data_out[4] = a11 / det; data_out[5] = a12 / det;
	data_out[6] = a20 / det; data_out[7] = a21 / det; data_out[8] = a22 / det;
}

template<typename T>
const T Operate(const Matrix3D__<T>& m, const Vector3D__<T>& f, const Vector3D__<T>& s)
{
	const T* data_m = (const T*)(&m);

	return	data_m[0] * f[0] * s[0] + data_m[1] * f[0] * s[1] + data_m[2] * f[0] * s[2] + 
			data_m[3] * f[1] * s[0] + data_m[4] * f[1] * s[1] + data_m[5] * f[1] * s[2] + 
			data_m[6] * f[2] * s[0] + data_m[7] * f[2] * s[1] + data_m[8] * f[2] * s[2];
}

//Difualt matrix3X3 types...

typedef Matrix3D__<Float>		Matrix3D;
typedef Matrix3D__<Double>	DMatrix3D;



#endif