#include <catch2/catch_amalgamated.hpp>
#include "../vmlib/mat44.hpp"

TEST_CASE("Matrix multiplication", "[mat44]")
{
	static constexpr float kEps_ = 1e-6f;

	using namespace Catch::Matchers;

	SECTION("4x4 [Identity] * 4x4 [Identity]")
	{
		auto const identity = Mat44f( Mat44f{ 1.f , 0.f, 0.f, 0.f,
											  0.f , 1.f, 0.f, 0.f, 
											  0.f , 0.f, 1.f, 0.f,
											  0.f , 0.f, 0.f, 1.f }

														*

									  Mat44f{ 1.f , 0.f, 0.f, 0.f,
											  0.f , 1.f, 0.f, 0.f,
											  0.f , 0.f, 1.f, 0.f,
											  0.f , 0.f, 0.f, 1.f } );
		
		REQUIRE_THAT(identity(0, 0), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(identity(0, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(0, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(0, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(identity(1, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(1, 1), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(identity(1, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(1, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(identity(2, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(2, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(2, 2), WithinAbs(1.f, kEps_));
		REQUIRE_THAT(identity(2, 3), WithinAbs(0.f, kEps_));

		REQUIRE_THAT(identity(3, 0), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(3, 1), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(3, 2), WithinAbs(0.f, kEps_));
		REQUIRE_THAT(identity(3, 3), WithinAbs(1.f, kEps_));
	}

	SECTION("4x4 [Custom] * 4x4 [Custom]")
	{
		auto const identity = Mat44f(Mat44f{ 2.f , 2.f, 2.f, 2.f,
											  4.f , 4.f, 4.f, 4.f,
											  6.f , 6.f, 6.f, 6.f,
											  8.f, 8.f, 8.f, 8.f }

													*

									Mat44f{ 8.f, 8.f, 8.f, 8.f,
											6.f , 6.f, 6.f, 6.f,
											4.f , 4.f, 4.f, 4.f,
											2.f , 2.f, 2.f, 2.f });

		REQUIRE_THAT(identity(0, 0), WithinAbs(40.f, kEps_));
		REQUIRE_THAT(identity(0, 1), WithinAbs(40.f, kEps_));
		REQUIRE_THAT(identity(0, 2), WithinAbs(40.f, kEps_));
		REQUIRE_THAT(identity(0, 3), WithinAbs(40.f, kEps_));

		REQUIRE_THAT(identity(1, 0), WithinAbs(80.f, kEps_));
		REQUIRE_THAT(identity(1, 1), WithinAbs(80.f, kEps_));
		REQUIRE_THAT(identity(1, 2), WithinAbs(80.f, kEps_));
		REQUIRE_THAT(identity(1, 3), WithinAbs(80.f, kEps_));

		REQUIRE_THAT(identity(2, 0), WithinAbs(120.f, kEps_));
		REQUIRE_THAT(identity(2, 1), WithinAbs(120.f, kEps_));
		REQUIRE_THAT(identity(2, 2), WithinAbs(120.f, kEps_));
		REQUIRE_THAT(identity(2, 3), WithinAbs(120.f, kEps_));

		REQUIRE_THAT(identity(3, 0), WithinAbs(160.f, kEps_));
		REQUIRE_THAT(identity(3, 1), WithinAbs(160.f, kEps_));
		REQUIRE_THAT(identity(3, 2), WithinAbs(160.f, kEps_));
		REQUIRE_THAT(identity(3, 3), WithinAbs(160.f, kEps_));
	}

	SECTION("4x4 [Identity] * 4x1 [Custom]")
	{
		auto const vector = Vec4f( Mat44f{    1.f , 0.f, 0.f, 0.f,
											  0.f , 1.f, 0.f, 0.f,
											  0.f , 0.f, 1.f, 0.f,
											  0.f , 0.f, 0.f, 1.f }

														*

									   Vec4f{ 2.f , 4.f, 8.f, 16.f });

		REQUIRE_THAT(vector[0], WithinAbs(2.f, kEps_));
		REQUIRE_THAT(vector[1], WithinAbs(4.f, kEps_)); 
		REQUIRE_THAT(vector[2], WithinAbs(8.f, kEps_)); 
		REQUIRE_THAT(vector[3], WithinAbs(16.f, kEps_));
	}
}

TEST_CASE("X-axis rotation", "[mat44]")
{
	static constexpr float kEps_ = 1e-6f;

	using namespace Catch::Matchers;

	SECTION("Rotate x-coordinate of identity vector by 45 degrees")
	{
		Vec4f vector = Vec4f{ 1.f, 0.f, 0.f, 1.f }; 
		Mat44f rotationMatrix = make_rotation_x(45.f);
		auto const result = rotationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(1.f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}

	SECTION("Rotate x coordinate of identity vector by 90 degrees")
	{
		Vec4f vector = Vec4f{ 1.f, 0.f, 0.f, 1.f };
		Mat44f rotationMatrix = make_rotation_x(90.f);
		auto const result = rotationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(1.f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}

	SECTION("Rotate x coordinate of identity vector by 180 degrees")
	{
		Vec4f vector = Vec4f{ 1.f, 0.f, 0.f, 1.f };
		Mat44f rotationMatrix = make_rotation_x(180.f);
		auto const result = rotationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(1.f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}

	SECTION("Rotate x coordinate of identity vector by 270 degrees")
	{
		Vec4f vector = Vec4f{ 1.f, 0.f, 0.f, 1.f };
		Mat44f rotationMatrix = make_rotation_x(270.f);
		auto const result = rotationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(1.f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}
}

TEST_CASE("Y-axis rotation", "[mat44]")
{
	static constexpr float kEps_ = 1e-3f;

	using namespace Catch::Matchers;

	SECTION("Rotate y-coordinate of identity vector by 45 degrees")
	{
		Vec4f vector = Vec4f{ 1.f, 0.f, 0.f, 1.f };
		Mat44f rotationMatrix = make_rotation_y(45.f);
		auto const result = rotationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(0.525f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(-0.850f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}

	SECTION("Rotate y-coordinate of identity vector by 90 degrees")
	{
		Vec4f vector = Vec4f{ 1.f, 0.f, 0.f, 1.f };
		Mat44f rotationMatrix = make_rotation_y(90.f);
		auto const result = rotationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(-0.448f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(-0.893f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}

	SECTION("Rotate y-coordinate of identity vector by 180 degrees")
	{
		Vec4f vector = Vec4f{ 1.f, 0.f, 0.f, 1.f };
		Mat44f rotationMatrix = make_rotation_y(180.f);
		auto const result = rotationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(-0.598f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(0.801f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}				 

	SECTION("Rotate y-coordinate of identity vector by 270 degrees")
	{
		Vec4f vector = Vec4f{ 1.f, 0.f, 0.f, 1.f };
		Mat44f rotationMatrix = make_rotation_y(270.f);
		auto const result = rotationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(0.984f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(0.176f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}
}

TEST_CASE("Z-axis rotation", "[mat44]")
{
	static constexpr float kEps_ = 1e-3f;

	using namespace Catch::Matchers;

	SECTION("Rotate z-coordinate of identity vector by 45 degrees")
	{
		Vec4f vector = Vec4f{ 1.f, 0.f, 0.f, 1.f };
		Mat44f rotationMatrix = make_rotation_z(45.f);
		auto const result = rotationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(0.5253f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(0.8509f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}

	SECTION("Rotate z-coordinate of identity vector by 90 degrees")
	{
		Vec4f vector = Vec4f{ 1.f, 0.f, 0.f, 1.f };
		Mat44f rotationMatrix = make_rotation_z(90.f);
		auto const result = rotationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(-0.4480f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(0.8939f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}

	SECTION("Rotate z-coordinate of identity vector by 180 degrees")
	{
		Vec4f vector = Vec4f{ 1.f, 0.f, 0.f, 1.f };
		Mat44f rotationMatrix = make_rotation_z(180.f);
		auto const result = rotationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(-0.5984f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(-0.8011f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}

	SECTION("Rotate z-coordinate of identity vector by 270 degrees")
	{
		Vec4f vector = Vec4f{ 1.f, 0.f, 0.f, 1.f };
		Mat44f rotationMatrix = make_rotation_z(270.f);
		auto const result = rotationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(0.9843f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(-0.1760f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(0.f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}
}

TEST_CASE("Matrix translation", "[mat44]")
{
	static constexpr float kEps_ = 1e-3f;

	using namespace Catch::Matchers;

	SECTION("Translation along the x")
	{
		Vec4f vector = Vec4f{ 1.f, 1.f, 1.f, 1.f };
		Mat44f translationMatrix = make_translation(Vec3f{3.f, 0.f, 0.f});
		auto const result = translationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(4.f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(1.f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(1.f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}

	SECTION("Translation along the y")
	{
		Vec4f vector = Vec4f{ 1.f, 1.f, 1.f, 1.f };
		Mat44f translationMatrix = make_translation(Vec3f{ 0.f, 3.f, 0.f });
		auto const result = translationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(1.f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(4.f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(1.f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}

	SECTION("Translation along the z")
	{
		Vec4f vector = Vec4f{ 1.f, 1.f, 1.f, 1.f };
		Mat44f translationMatrix = make_translation(Vec3f{ 0.f, 0.f, 3.f });
		auto const result = translationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(1.f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(1.f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(4.f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}

	SECTION("Translation along the x, y and z axes")
	{
		Vec4f vector = Vec4f{ 1.f, 1.f, 1.f, 1.f };
		Mat44f translationMatrix = make_translation(Vec3f{ 3.f, 3.f, 3.f });
		auto const result = translationMatrix * vector;

		REQUIRE_THAT(result[0], WithinAbs(4.f, kEps_));
		REQUIRE_THAT(result[1], WithinAbs(4.f, kEps_));
		REQUIRE_THAT(result[2], WithinAbs(4.f, kEps_));
		REQUIRE_THAT(result[3], WithinAbs(1.f, kEps_));
	}
}