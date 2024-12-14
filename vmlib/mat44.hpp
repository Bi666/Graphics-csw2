#ifndef MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
#define MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "vec3.hpp"
#include "vec4.hpp"

/** Mat44f: 4x4 matrix with floats
 *
 * See vec2f.hpp for discussion. Similar to the implementation, the Mat44f is
 * intentionally kept simple and somewhat bare bones.
 *
 * The matrix is stored in row-major order (careful when passing it to OpenGL).
 *
 * The overloaded operator () allows access to individual elements. Example:
 *    Mat44f m = ...;
 *    float m12 = m(1,2);
 *    m(0,3) = 3.f;
 *
 * The matrix is arranged as:
 *
 *   ⎛ 0,0  0,1  0,2  0,3 ⎞
 *   ⎜ 1,0  1,1  1,2  1,3 ⎟
 *   ⎜ 2,0  2,1  2,2  2,3 ⎟
 *   ⎝ 3,0  3,1  3,2  3,3 ⎠
 */
struct Mat44f
{
	float v[16];

	constexpr
	float& operator() (std::size_t aI, std::size_t aJ) noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
	constexpr
	float const& operator() (std::size_t aI, std::size_t aJ) const noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
};

// Identity matrix
constexpr Mat44f kIdentity44f = { {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
} };

// Common operators for Mat44f.
// Note that you will need to implement these yourself.

constexpr
Mat44f operator*( Mat44f const& aLeft, Mat44f const& aRight ) noexcept
{
	Mat44f result = { {} };

	for (std::size_t i = 0; i < 4; ++i)
	{
		for (std::size_t j = 0; j < 4; ++j)
		{
			result(i, j) = 0.0f;
			for (std::size_t k = 0; k < 4; ++k)
			{
				result(i, j) += aLeft(i, k) * aRight(k, j);
			}
		}
	}

	return result;
}

constexpr
Vec4f operator*( Mat44f const& aLeft, Vec4f const& aRight ) noexcept
{
	Vec4f result =
	{
		(aLeft(0,0) * aRight.x) + (aLeft(0, 1) * aRight.y) + (aLeft(0, 2) * aRight.z) + (aLeft(0, 3) * aRight.w), //x
		(aLeft(1,0) * aRight.x) + (aLeft(1, 1) * aRight.y) + (aLeft(1, 2) * aRight.z) + (aLeft(1, 3) * aRight.w), //y
		(aLeft(2,0) * aRight.x) + (aLeft(2, 1) * aRight.y) + (aLeft(2, 2) * aRight.z) + (aLeft(2, 3) * aRight.w), //z
		(aLeft(3,0) * aRight.x) + (aLeft(3, 1) * aRight.y) + (aLeft(3, 2) * aRight.z) + (aLeft(3, 3) * aRight.w)  //w
	};

	return result;
}

inline
Mat44f make_rotation_x( float aAngle ) noexcept
{
	Mat44f rotationMatrix = kIdentity44f;


	// Get the cosine and sine of 'aAngle'
	float cosTheta = std::cos(aAngle);
	float sinTheta = std::sin(aAngle);

	// Update the rotation components
	rotationMatrix(1, 1) = cosTheta;
	rotationMatrix(1, 2) = -sinTheta;
	rotationMatrix(2, 1) = sinTheta;
	rotationMatrix(2, 2) = cosTheta;

	return rotationMatrix;
}


inline
Mat44f make_rotation_y( float aAngle ) noexcept
{
	Mat44f rotationMatrix = kIdentity44f;

	float cosTheta = std::cos(aAngle);
	float sinTheta = std::sin(aAngle);

	rotationMatrix(0, 0) = cosTheta;
	rotationMatrix(0, 2) = sinTheta;
	rotationMatrix(2, 0) = -sinTheta;
	rotationMatrix(2, 2) = cosTheta;

	return rotationMatrix;
}

inline
Mat44f make_rotation_z( float aAngle ) noexcept
{
	Mat44f rotationMatrix = kIdentity44f;

	float cosTheta = std::cos(aAngle);
	float sinTheta = std::sin(aAngle);

	rotationMatrix(0, 0) = cosTheta;
	rotationMatrix(0, 1) = -sinTheta;
	rotationMatrix(1, 0) = sinTheta;
	rotationMatrix(1, 1) = cosTheta;

	return rotationMatrix;
}

inline
Mat44f make_translation( Vec3f aTranslation ) noexcept
{
	Mat44f translationMatrix = kIdentity44f;

	translationMatrix(0, 3) = aTranslation.x;
	translationMatrix(1, 3) = aTranslation.y;
	translationMatrix(2, 3) = aTranslation.z;

	return translationMatrix;
}

inline
Mat44f make_scaling( float aSX, float aSY, float aSZ ) noexcept
{
	//TODO: your implementation goes here
	//TODO: remove the following when you start your implementation
	(void)aSX;  // Avoid warnings about unused arguments until the function
	(void)aSY;  // is properly implemented.
	(void)aSZ;
	return kIdentity44f;
}


inline
Mat44f make_perspective_projection( float aFovInRadians, float aAspect, float aNear, float aFar ) noexcept
{
	if (aAspect == 0)
	{
		return kIdentity44f;
	}

	float s = 1.0f / std::tan(aFovInRadians / 2.0f);

	Mat44f perspectiveprojectionMatrix = kIdentity44f;

	perspectiveprojectionMatrix(0, 0) = s / aAspect;
	perspectiveprojectionMatrix(1, 1) = s;
	perspectiveprojectionMatrix(2, 2) = -(aFar + aNear) / (aFar - aNear);
	perspectiveprojectionMatrix(2, 3) = -2.0f * (aFar * aNear) / (aFar - aNear);
	perspectiveprojectionMatrix(3, 2) = -1.0f;
	perspectiveprojectionMatrix(3, 3) = 0.f;

	return perspectiveprojectionMatrix;
}

Mat44f invert(Mat44f const& aM) noexcept;

inline
Mat44f transpose(Mat44f const& aM) noexcept
{
	Mat44f ret;
	for (std::size_t i = 0; i < 4; ++i)
	{
		for (std::size_t j = 0; j < 4; ++j)
			ret(j, i) = aM(i, j);
	}
	return ret;
}



#endif // MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
