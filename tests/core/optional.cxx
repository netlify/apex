#include <apex/core/optional.hpp>
#include <string_view>

TEST_CASE("default-constructor") {
  [[maybe_unused]] constexpr apex::optional<int> test { };
  CHECK_FALSE(test);
}

TEST_CASE("reference-constructor") {
  int x = 42;
  [[maybe_unused]] apex::optional<int&> test { x };
  CHECK(std::addressof(*test) == std::addressof(x));
}

TEST_CASE("try-emplace") {
  apex::optional<int> x { 42 };
  auto&& value = x.try_emplace(54);
  CHECK(value == 42);
}

TEST_CASE("emplace") {
  apex::optional<int> x { };
  CHECK_FALSE(x);
  x.emplace(42);
  CHECK(x);
  CHECK(x == 42);
}

TEST_CASE("equal-to") {
  constexpr apex::optional<int> lhs { };
  constexpr apex::optional<int> rhs { };
  CHECK(lhs == rhs);
}

TEST_CASE("not-equal-to") {
  constexpr apex::optional<int> lhs { 42 };
  constexpr apex::optional<int> rhs { };
  CHECK(lhs != rhs);
}

TEST_CASE("less") {
  constexpr apex::optional<int> lhs { };
  constexpr apex::optional<int> rhs { 42 };
  CHECK(lhs < rhs);
}

TEST_CASE("greater") {
  constexpr apex::optional<int> lhs { 42 };
  constexpr apex::optional<int> rhs { };
  CHECK(lhs > rhs);
}

TEST_CASE("reference-equal-to") {
  int x = 42;
  apex::optional<int&> lhs { x };
  apex::optional<int&> rhs { x };
  CHECK(lhs == rhs);
}

TEST_CASE("transform") {
  apex::optional<int> optional { };
  auto result = optional.transform([] (int i) { return i + i; });
  STATIC_REQUIRE(apex::same_as<decltype(result), apex::optional<int>>);
  REQUIRE(not result);
}
