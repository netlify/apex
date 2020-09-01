#include <apex/core/array.hpp>

TEST_CASE("basic-c-array") {
  static constexpr int x[4] = { 1, 2, 3, 4 };
  static constexpr auto converted = ::apex::to_array(x);
  STATIC_REQUIRE(converted[0] == x[0]);
  STATIC_REQUIRE(converted[1] == x[1]);
  STATIC_REQUIRE(converted[2] == x[2]);
  STATIC_REQUIRE(converted[3] == x[3]);
}