#include <apex/core/optional.hpp>
#include <apex/core/string.hpp>

TEST_CASE("default-constructor") {
  [[maybe_unused]] constexpr apex::optional<int> test { };
  CHECK_FALSE(test);
}

TEST_CASE("copy-constructor") {
  apex::optional<int> opt { 4 };
  apex::optional<int> copy { opt };
  CHECK(opt);
  CHECK(copy);
  CHECK(opt == copy);
  CHECK(copy.value() == 4);
}

TEST_CASE("move-constructor") {
  apex::optional<int> opt { 4 };
  apex::optional<int> move { std::move(opt) };
  CHECK(opt); // should be in a valid, but null state
  CHECK(move);
  CHECK(move.value() == 4);
}

TEST_CASE("nullopt-constructor") {
  apex::optional<int> opt { std::nullopt };
  CHECK_FALSE(opt);
}

//TEST_CASE("copy-value-constructor") {
//  std::string text { "copy-value" };
//  apex::optional<std::string> opt { text };
//
//  CHECK_FALSE(text.empty());
//  CHECK(opt);
//  CHECK(opt.value() == text);
//  CHECK(opt.value() == "copy-value");
//}

//TEST_CASE("move-value-constructor") {
//  std::string text { "move-value" };
//  apex::optional<std::string> opt { std::move(text) };
//  CHECK(opt);
//  CHECK(text.empty());
//  CHECK(opt.value() == "move-value");
//}

TEST_CASE("reference-constructor") {
  int x = 42;
  [[maybe_unused]] apex::optional<int&> test { x };
  CHECK(std::addressof(*test) == std::addressof(x));
}

TEST_CASE("copy-assignment") {
  apex::optional<int> opt { 4 };
  apex::optional<int> copy { };
  copy = opt;

  REQUIRE(opt);
  REQUIRE(copy);
  CHECK(opt == copy);
  CHECK(*copy == 4);
}

TEST_CASE("move-assignment") {
  apex::optional<int> opt { 4 };
  apex::optional<int> move { };
  move = std::move(opt);

  CHECK(opt); // in a valid, but null state
  CHECK(move);
  CHECK(*move == 4);
}

//TEST_CASE("copy-value-assignment") {
//  apex::optional<std::string> opt { };
//  std::string value { "copy-value" };
//  opt = value;
//  CHECK(value.empty());
//  CHECK(opt);
//  CHECK(*opt == "copy-value");
//}
//
//TEST_CASE("move-value-assignment") {
//  apex::optional<std::string> opt { };
//  std::string value { "move-value" };
//  opt = std::move(value);
//  CHECK(value.empty());
//  CHECK(opt);
//  CHECK(*opt == "move-value");
//}


//TEST_CASE("arrow-operator") {
//  apex::optional<std::string> opt { "arrow" };
//  CHECK(opt);
//  CHECK_FALSE(opt->empty());
//}

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
