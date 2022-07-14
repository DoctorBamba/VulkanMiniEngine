#pragma once

#include "PE_Matrix3X3.inl"
#include "PE_Matrix4X4.inl"
#include "PE_Quaterrnion.inl"

#ifndef __LINEAR_MATH_TRANSFORMS_HEADER__
#define __LINEAR_MATH_TRANSFORMS_HEADER__

template<typename T>
inline const Matrix3D__<T> Translate2D(const Vector2D__<T>& position);

template<typename T>
inline const Matrix3D__<T> Scale2D(const Vector2D__<T>& scale);

template<typename T>
inline const Matrix3D__<T> Rotate2D(const Float angle);

template<typename T>
const Matrix4D__<T> Translate(const Vector3D__<T>& position);

template<typename T>
const Matrix4D__<T> Scale(const Vector3D__<T>& scale);

template<typename T>
const Matrix4D__<T> Rotate(const Quaternion__<T>& quat);

template<typename T>
const Quaternion__<T> Orientation(const Matrix4D__<T>& ortho_matrix);

template<typename T>
const Matrix4D__<T> LookAt(const Vector3D__<T>& position, const Vector3D__<T>& focus, const Vector3D__<T>& up);

template<typename T>
const Matrix4D__<T> Perspective(T angle_, T aspect_, T near_, T far_);

template<class T>
Void DismantleTransform(const Matrix4D__<T>& transform, Vector3D__<T>* position_out, Vector3D__<T>* scale_out, Matrix3D__<T>* orientation_out);

template<class T>
Void DismantleTransform(const Matrix4D__<T>& transform, Vector3D__<T>* position_out, Vector3D__<T>* scale_out, Quaternion__<T>* orientation_out);

template<class T>
Quaternion__<T> Interpolate(const Quaternion__<T>& start, const Quaternion__<T>& end, Float t);

template<class T>
Matrix4D__<T> Interpolate(const Matrix4D__<T>& start, const Matrix4D__<T>& end, Float t);

template<typename T>
const Matrix4D__<T> Make4x4(const Matrix3D__<T>& transform3x3);

template<typename T>
const Matrix3D__<T> Make3x3(const Matrix4D__<T>& transform4x4);

template<class T>
const Vector3D__ <T> Make3D(const Vector4D__<T>& vector4D);

template<class T>
const Vector4D__ <T> Make4D(const Vector3D__<T>& vector3D);

#endif