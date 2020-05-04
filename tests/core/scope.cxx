#include <catch2/catch.hpp>

#include <apex/core/scope.hpp>

TEST_CASE("scope_exit execute") {
  int x = 0;
  {
    apex::scope_exit scope {
      [&] () mutable { x = 1; }
    };
  }
  REQUIRE(x == 1);
}

TEST_CASE("scope_exit release") {
  int x = 0;
  {
    apex::scope_exit scope {
      [&] () mutable { x = 1; }
    };
    scope.release();
  }
  REQUIRE(x == 0);
}

TEST_CASE("scope_success execute") { }
TEST_CASE("scope_success release") { }
TEST_CASE("scope_failure execute") { }
TEST_CASE("scope_failure release") { }
