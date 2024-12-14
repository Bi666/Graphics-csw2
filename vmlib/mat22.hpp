#ifndef MAT22_HPP_1F974C02_D0D1_4FBD_B5EE_A69C88112088
#define MAT22_HPP_1F974C02_D0D1_4FBD_B5EE_A69C88112088

#include <cmath>

#include "vec2.hpp"

/** Mat22f : 2x2 matrix with floats
 *
 * See comments for Vec2f for some discussion.
 *
 * The matrix is stored in row-major order.
 *
 * Example:
 *   Mat22f identity{ 
 *     1.f, 0.f,
 *     0.f, 1.f
 *   };
 */
struct Mat22f
{
	float _00, _01;
	float _10, _11;
};

// Common operators for Mat22f.
// Note that you will need to implement these yourself.

constexpr
Mat22f operator*( Mat22f const& aLeft, Mat22f const& aRight ) noexcept
{
	//we declare the matrix here
	Mat22f multipliedMatrix
	{
	};

	//accessing mat22f matrix notes
	// ._00 TOP LEFT   ._01 TOP RIGHT
	// ._10 TOP RIGHT  ._11 TOP LEFT

	//TOP LEFT
	multipliedMatrix._00 = (aLeft._00 * aRight._00) + (aLeft._01 * aRight._10);
	//TOP RIGHT
	multipliedMatrix._01 = (aLeft._00 * aRight._01) + (aLeft._01 * aRight._11);
	//BOT LEFT
	multipliedMatrix._10 = (aLeft._10 * aRight._00) + (aLeft._11 * aRight._10);
	//BOT RIGHT
	multipliedMatrix._11 = (aLeft._10 * aRight._01) + (aLeft._11 * aRight._11);

	return multipliedMatrix; // Return the resulting matrix.
}

constexpr
Vec2f operator*( Mat22f const& aLeft, Vec2f const& aRight ) noexcept
{
	//we declare vector here
	Vec2f matrixvectorMultiplied
	{							};

	matrixvectorMultiplied.x = (aLeft._00 * aRight.x) + (aLeft._01 * aRight.y);
	matrixvectorMultiplied.y = (aLeft._10 * aRight.x) + (aLeft._11 * aRight.y);

	return matrixvectorMultiplied;
}

// Functions:

inline
Mat22f make_rotation_2d( float aAngle ) noexcept
{
	//declare matrix
	Mat22f rotationMatrix
	{
	};

	//here we get the cosine and sin of 'aAngle'
	float cosTheta = std::cos(aAngle);
	float sinTheta = std::sin(aAngle);

	rotationMatrix._00 = cosTheta;
	rotationMatrix._01 = -sinTheta;
	rotationMatrix._10 = sinTheta;
	rotationMatrix._11 = cosTheta;

	return rotationMatrix;
}

#endif // MAT22_HPP_1F974C02_D0D1_4FBD_B5EE_A69C88112088
