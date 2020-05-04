#include <catch2/catch.hpp>

#include <apex/core/functional.hpp>

TEST_CASE("bind_front") {
  constexpr auto call = apex::bind_front(std::move([] (auto&& x, auto&& y) {
    return std::forward<decltype(x)>(x) + std::forward<decltype(y)>(y);
  }), 4);
  REQUIRE(call(5) == 9);
}

TEST_CASE("function_ref (can throw)") {
  apex::function_ref<int(int)> call {
    [] (auto x) { return x * x; }
  };
  REQUIRE(call(2) == 4);
}
