#include <apex/core/utility.hpp>

TEST_CASE("to_underlying") {
  enum class check : bool {
    no,
    yes
  };

  REQUIRE_FALSE(apex::to_underlying(check::no));
  REQUIRE(apex::to_underlying(check::yes));
}

TEST_CASE("offset_to") {
  struct X {
    int x;
  };
  REQUIRE(apex::offset_to(&X::x) == 0);
}

TEST_CASE("overloaded") {
  auto call = apex::overloaded {
    [] (int) { return true; },
    [] (double) { return false; }
  };
  REQUIRE_FALSE(call(0.0));
  REQUIRE(call(0));
}
