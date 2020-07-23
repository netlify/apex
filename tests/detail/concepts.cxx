#include <apex/detail/prelude/concepts.hpp>

TEST_CASE("similar_to") {
  STATIC_REQUIRE(apex::similar_to<int, int const>);
  STATIC_REQUIRE(apex::similar_to<int, int const&>);
  STATIC_REQUIRE(apex::similar_to<int&, int const&>);
}

TEST_CASE("different_from") {
  STATIC_REQUIRE(apex::different_from<int, void>);
  STATIC_REQUIRE(not apex::different_from<std::add_lvalue_reference_t<int>, int>);
}

TEST_CASE("distinct_from") {
  STATIC_REQUIRE(apex::distinct_from<int, void>);
  STATIC_REQUIRE(apex::distinct_from<std::add_lvalue_reference_t<int>, int>);
}
