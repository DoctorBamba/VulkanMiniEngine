#pragma once

#include "PE_Transforms.h"

template<typename T>
inline const Matrix3D__<T> Translate2D(const Vector2D__<T>& position)
{
	return Matrix3D__<T>(T(1), T(),  position[0],
							T(),  T(1), position[1],
							T(),  T(),  T(1));
}

template<typename T>
inline const Matrix3D__<T> Scale2D(const Vector2D__<T>& scale)
{
	return Matrix3D__<T>(scale[0], T(),      T(),
							T(),      scale[1], T(),
							T(),      T(),      T(1));
}

template<typename T>
inline const Matrix3D__<T> Rotate2D(const Float angle)
{
	const Float c = cos(angle);
	const Float s = sin(angle);
	return Matrix3D__<T>(T(c), -T(s), T(),
							T(s),  T(c), T(),
							T(),   T(),  T(1));
}

template<typename T>
inline const Matrix4D__<T> Translate(const Vector3D__<T>& position)
{
	return Matrix4D__<T>(T(1), T(),  T(),  position[0],
							T(),  T(1), T(),  position[1],
							T(),  T(),  T(1), position[2],
							T(),  T(),  T(),  T(1));
}

template<typename T>
inline const Matrix4D__<T> Scale(const Vector3D__<T>& scale)
{
	return Matrix4D__<T>(scale[0], T(), T(), T(),
							T(), scale[1], T(), T(),
							T(), T(), scale[2], T(),
							T(), T(), T(),		T(1));
}

template<typename T>
inline const Matrix4D__<T> Rotate(const Quaternion__<T>& quat)
{
	const T tl = 2 / quat.norm();
	return Matrix4D__<T>(1 - tl * (quat[2] * quat[2] + quat[3] * quat[3]), tl * (quat[1] * quat[2] - quat[3] * quat[0]), tl * (quat[1] * quat[3] + quat[2] * quat[0]), T(),
							tl * (quat[1] * quat[2] + quat[3] * quat[0]), 1 - tl * (quat[1] * quat[1] + quat[3] * quat[3]), tl * (quat[2] * quat[3] - quat[1] * quat[0]), T(),
							tl * (quat[1] * quat[3] - quat[2] * quat[0]), tl * (quat[2] * quat[3] + quat[1] * quat[0]), 1 - tl * (quat[1] * quat[1] + quat[2] * quat[2]), T(),
							T()											, T()											, T()											, T(1));
}

template<typename T>
inline const Matrix4D__<T> Transform(const Vector3D__<T>& position, const Quaternion& orientation, const Vector3D__<T>& scale)
{
	const T* P = (T*)&(Rotate(orientation));

	return Matrix4D__<T>(P[0] * scale[0], P[1] * scale[1], P[2]  * scale[2], position[0],
							P[4] * scale[0], P[5] * scale[1], P[6]  * scale[2], position[1],
							P[8] * scale[0], P[9] * scale[1], P[10] * scale[2], position[2],
							T()				  , T()				  , T()				  ,	T(1.0f));
}				

template<typename T>
inline const Quaternion__<T> Orientation(const Matrix4D__<T>& ortho_matrix)
{
	Quaternion__<T> out;
	float v0_length = sqrt(ortho_matrix[0][0] * ortho_matrix[0][0] + ortho_matrix[1][0] * ortho_matrix[1][0] + ortho_matrix[2][0] * ortho_matrix[2][0]);
	float v1_length = sqrt(ortho_matrix[0][1] * ortho_matrix[0][1] + ortho_matrix[1][1] * ortho_matrix[1][1] + ortho_matrix[2][1] * ortho_matrix[2][1]);
	float v2_length = sqrt(ortho_matrix[0][2] * ortho_matrix[0][2] + ortho_matrix[1][2] * ortho_matrix[1][2] + ortho_matrix[2][2] * ortho_matrix[2][2]);
	out[0] = sqrt(1.0f + ortho_matrix[0][0] / v0_length + ortho_matrix[1][1] / v1_length + ortho_matrix[2][2] / v2_length) * 0.5;
	if (out[0] == T())
		return Quaternion__<T>(T(1), T(), T(), T());
	out[1] = (ortho_matrix[2][1] / v1_length - ortho_matrix[1][2] / v2_length) / (4.0f * out[0]);
	out[2] = (ortho_matrix[0][2] / v2_length - ortho_matrix[2][0] / v0_length) / (4.0f * out[0]);
	out[3] = (ortho_matrix[1][0] / v0_length - ortho_matrix[0][1] / v1_length) / (4.0f * out[0]);

	return out * (T(1) / sqrt(out.norm()));
}


template<typename T>
const Matrix4D__<T> Make4x4(const Matrix3D__<T>& transform3x3)
{
	return Matrix4D__<T>(transform3x3[0][0], transform3x3[0][1], transform3x3[0][2], T(),
							transform3x3[1][0], transform3x3[1][1], transform3x3[1][2], T(),
							transform3x3[2][0], transform3x3[2][1], transform3x3[2][2], T(),
							T()				  , T()				  , T()			      , T(1));
}

template<typename T>
const Matrix3D__<T> Make3x3(const Matrix4D__<T>& transform3x3)
{
	return Matrix3D__<T>(transform3x3[0][0], transform3x3[0][1], transform3x3[0][2],
							transform3x3[1][0], transform3x3[1][1], transform3x3[1][2],
							transform3x3[2][0], transform3x3[2][1], transform3x3[2][2]);
}

template<class T>
const Vector3D__ <T> Make3D(const Vector4D__<T>& vector4D)
{
	return Vector3D__<T>(vector4D[0], vector4D[1], vector4D[2]);
}

template<class T>
const Vector4D__ <T> Make4D(const Vector3D__<T>& vector3D)
{
	return Vector4D__<T>(vector3D[0], vector3D[1], vector3D[2], T(1));
}


template<typename T>
const Matrix4D__<T> LookAt(const Vector3D__<T>& position, const Vector3D__<T>& focus, const Vector3D__<T>& up)
{
	//Define the view-space base...
	Vector3D__<T> e2 = (focus - position);
	e2 = e2 * (1.0f / e2.lenght());

	Vector3D__<T> e0 = e2 ^ up;
	e0 = e0 * (1.0f / e0.lenght());

	Vector3D__<T> e1 = e0 ^ e2;

	//Calculate the coordination of position in the view space...
	const Vector3D__<T> position_in_viewspace(position.dot(e0), position.dot(e1), position.dot(e2));

	//retrun the 4d matrix of the invers-base-transform matrix. Use the idententy of orthonormal bases - T^(-1) = T*
	return Matrix4D__<T>(e0[0], e0[1], e0[2], -position_in_viewspace[0],
						 -e1[0], -e1[1], -e1[2], position_in_viewspace[1],
						 e2[0], e2[1], e2[2], -position_in_viewspace[2],
						 0.0f, 0.0f, 0.0f, 1.0f);
}

template<typename T>
const Matrix4D__<T> Perspective(T angle_, T aspect_, T near_, T far_)
{
	const T f = 1.0f / tan(angle_ * 0.5f);
	return Matrix4D__<T>(f, 0.0f, 0.0f, 0.0f,
							0.0f, f, 0.0f, 0.0f,
							0.0f, 0.0f, far_ / (far_ - near_), -(near_ * far_) / (far_ - near_),
							0.0f, 0.0f, 1.0f, 0.0f);
}

template<class T>
Void DismantleTransform(const Matrix4D__<T>& transform, Vector3D__<T>* position_out, Vector3D__<T>* scale_out, Matrix3D__<T>* orientation_out)
{
	Float d1 = sqrt(transform[0][0] * transform[0][0] + transform[1][0] * transform[1][0] + transform[2][0] * transform[2][0]);
	Float d2 = sqrt(transform[0][1] * transform[0][1] + transform[1][1] * transform[1][1] + transform[2][1] * transform[2][1]);
	Float d3 = sqrt(transform[0][2] * transform[0][2] + transform[1][2] * transform[1][2] + transform[2][2] * transform[2][2]);

	*position_out		= Vector3D(transform[0][3], transform[1][3], transform[2][3]);
	*scale_out			= PE_Vector3D(d1, d2, d3);
	*orientation_out	= Matrix3D(transform[0][0] / d1, transform[0][1] / d2, transform[0][2] / d3,
									  transform[1][0] / d1, transform[1][1] / d2, transform[1][2] / d3,
									  transform[2][0] / d1, transform[2][1] / d2, transform[2][2] / d3);
}

template<class T>
Void DismantleTransform(const Matrix4D__<T>& transform, Vector3D__<T>* position_out, Vector3D__<T>* scale_out, Quaternion__<T>* orientation_out)
{
	Float d0 = sqrt(transform[0][0] * transform[0][0] + transform[1][0] * transform[1][0] + transform[2][0] * transform[2][0]);
	Float d1 = sqrt(transform[0][1] * transform[0][1] + transform[1][1] * transform[1][1] + transform[2][1] * transform[2][1]);
	Float d2 = sqrt(transform[0][2] * transform[0][2] + transform[1][2] * transform[1][2] + transform[2][2] * transform[2][2]);
	T w = static_cast<T>(sqrt(1.0f + transform[0][0] / d0 + transform[1][1] / d1 + transform[2][2] / d2) / 2.0f);
	
	*position_out		= Vector3D(transform[0][3], transform[1][3], transform[2][3]);
	*scale_out			= PE_Vector3D(d0, d1, d2);

	(*orientation_out)[0]   = w;
	(*orientation_out)[1]  = (transform[2][1] / d1 - transform[1][2] / d2) / (4 * w);
	(*orientation_out)[2]  = (transform[0][2] / d2 - transform[2][0] / d0) / (4 * w);
	(*orientation_out)[3]  = (transform[1][0] / d0 - transform[0][1] / d1) / (4 * w);
}


//Taked from assimp...
template<class T>
Quaternion__<T> Interpolate(const Quaternion__<T>& start, const Quaternion__<T>& end, Float t)
{
	Float cos_teta = start[0] * end[0] + start[1] * end[1] + start[2] * end[2] + start[3] * end[3];
	
	Quaternion__<T> e = end;
	if (cos_teta < 0.0f)
	{
		cos_teta = -cos_teta;
		e = -end;
	}

	// Calculate coefficients
	Float sclp, sclq;
	if ((1.0f - cos_teta) > 0.0001)
	{
		// Standard case (slerp)
		Float teta = std::acos(cos_teta); // extract theta from dot product's cos theta
		Float sin_teta = std::sin(teta);
		sclp = std::sin((1.0 - t) * teta) / sin_teta;
		sclq = std::sin(t * teta) / sin_teta;
	}
	else
	{
		// Very close, do linear interp (because it's faster)
		sclp = 1.0 - t;
		sclq = t;
	}

	return sclp * start + sclq * e;
}

template<class T>
Matrix4D__<T> Interpolate(const Matrix4D__<T>& start, const Matrix4D__<T>& end, Float t)
{
	Vector3D__<T>	start_position;
	Quaternion__<T>	start_orientation;
	Vector3D__<T>	start_scale;
	DismantleTransform(start, &start_position, &start_scale, &start_orientation);

	Vector3D__<T>	end_position;
	Quaternion__<T>	end_orientation;
	Vector3D__<T>	end_scale;
	DismantleTransform(end, &end_position, &end_scale, &end_orientation);


	return Transform((1.0f - t) * start_position + t * end_position,
						Interpolate(start_orientation, end_orientation, t),
						(1.0f - t) * start_scale + t * end_scale);
}