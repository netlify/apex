#include <apex/core/bit.hpp>
#include <limits>

TEST_CASE("popcount") {
  static constexpr auto digits = std::numeric_limits<unsigned int>::digits;
  static constexpr auto max = std::numeric_limits<unsigned int>::max();

  REQUIRE(apex::popcount(max) == digits);
}
