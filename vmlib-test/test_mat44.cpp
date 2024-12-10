#define CATCH_CONFIG_MAIN
#include <catch2/catch_amalgamated.hpp>
#include "../vmlib/mat44.hpp"
using namespace Catch;

TEST_CASE("Matrix multiplication", "[mat44]") {
    Mat44f m1 = kIdentity44f;
    Mat44f m2 = kIdentity44f;

    m1(0, 0) = 2.0f;
    m2(1, 1) = 3.0f;

    Mat44f result = m1 * m2;

    REQUIRE(result(0, 0) == Catch::Approx(2.0f));
    REQUIRE(result(1, 1) == Catch::Approx(3.0f));
    REQUIRE(result(2, 2) == Catch::Approx(1.0f));
    REQUIRE(result(3, 3) == Catch::Approx(1.0f));
}

TEST_CASE("Matrix-vector multiplication", "[mat44]") {
    Mat44f m = kIdentity44f;
    Vec4f v = { 1.0f, 2.0f, 3.0f, 1.0f };

    m(0, 0) = 2.0f;
    m(1, 1) = 3.0f;

    Vec4f result = m * v;

    REQUIRE(result[0] == Catch::Approx(2.0f));
    REQUIRE(result[1] == Catch::Approx(6.0f));
    REQUIRE(result[2] == Catch::Approx(3.0f));
    REQUIRE(result[3] == Catch::Approx(1.0f));
}

TEST_CASE("Rotation matrix generation", "[mat44]") {
    Mat44f rotation = make_rotation_x(3.14159f / 2); // 90 degrees around X-axis
    Vec4f v = { 0.0f, 1.0f, 0.0f, 1.0f };           // Vector pointing along Y-axis

    Vec4f result = rotation * v;

    REQUIRE(result[0] == Catch::Approx(0.0f).margin(0.001f));
    REQUIRE(result[1] == Catch::Approx(0.0f).margin(0.001f));
    REQUIRE(result[2] == Catch::Approx(1.0f).margin(0.001f)); // Now pointing along Z-axis
    REQUIRE(result[3] == Catch::Approx(1.0f).margin(0.001f));
}

TEST_CASE("Perspective projection matrix", "[mat44]") {
    Mat44f projection = make_perspective_projection(3.14159f / 4, 16.0f / 9.0f, 0.1f, 100.0f);
    Vec4f v = { 0.0f, 0.0f, -1.0f, 1.0f };

    Vec4f result = projection * v;

    REQUIRE(result[0] == Catch::Approx(0.0f));
    REQUIRE(result[1] == Catch::Approx(0.0f));
    REQUIRE(result[2] >= -1.0f); // Z 在裁剪空间范围内
    REQUIRE(result[2] <= 1.0f);  // Z 在裁剪空间范围内
    REQUIRE(result[3] > 0.001f); // 齐次坐标大于 0
}

TEST_CASE("Translation matrix generation", "[mat44]") {
    Mat44f translation = make_translation({2.0f, 3.0f, 4.0f});
    Vec4f v = {1.0f, 1.0f, 1.0f, 1.0f};
    Vec4f result = translation * v;
    
    REQUIRE(result[0] == Catch::Approx(v[0] + 2.0f)); // x 平移累加
    REQUIRE(result[1] == Catch::Approx(v[1] + 3.0f)); // y 平移累加
    REQUIRE(result[2] == Catch::Approx(v[2] + 4.0f)); // z 平移累加
    REQUIRE(result[3] == Catch::Approx(v[3]));        // w 不变
}