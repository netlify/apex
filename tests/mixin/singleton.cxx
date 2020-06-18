#include <apex/mixin/singleton.hpp>

TEST_CASE("global") {
  auto lhs = std::addressof(apex::global<int const>);
  auto rhs = std::addressof(apex::singleton<int>::global());
  REQUIRE(lhs == rhs);
}

TEST_CASE("global vs local") {
  auto lhs = std::addressof(apex::global<int const>);
  auto rhs = std::addressof(apex::local<int const>);
  REQUIRE(lhs != rhs);
}
