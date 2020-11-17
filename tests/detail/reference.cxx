#include <apex/detail/prelude/reference.hpp>
#include <apex/core/traits.hpp>


constexpr bool is_lvalue (apex::detail::prelude::lvalue_reference auto&&) { return true; }
constexpr bool is_lvalue (auto) { return false; }

TEST_CASE("rvalue-reference") {
  STATIC_REQUIRE(apex::detail::prelude::rvalue_reference<int&&>);
}

TEST_CASE("lvalue-reference") {
  STATIC_REQUIRE(apex::detail::prelude::lvalue_reference<int&>);
}

TEST_CASE("subsumption") {
  int x { };
  STATIC_REQUIRE(::is_lvalue(x));
}
