#include <apex/core/either.hpp>
#include <apex/core/string.hpp>

/* TODO: The set of tests are currently difficult to add or remove. We should
 * rely on adding a few extra sections since this is such an important part of
 * the apex core library
 */

struct test {
  test(std::string& reference) noexcept : reference(reference) { }
  ~test() { this->reference.clear(); }
  std::string& reference;
};

TEST_CASE("in-place-index-value-constructor") {
  apex::either<int, int> integer { std::in_place_index<0>, 4 };
  CHECK(integer.index() == 0);
  CHECK(integer.has_value());
  CHECK(integer.assume_value() == 4);
}

TEST_CASE("in-place-index-other-constructor") {
  apex::either<int, int> integer { std::in_place_index<1>, 4 };
  CHECK(integer.index() == 1);
  CHECK(integer.has_other());
  CHECK(integer.assume_other() == 4);
}

TEST_CASE("in-place-type-value-constructor") {
  apex::either<int, float> integer { std::in_place_type<int>, 4 };
  CHECK(integer.index() == 0);
  CHECK(integer.has_value());
  CHECK(integer.assume_value() == 4);
}

TEST_CASE("in-place-type-other-constructor") {
  apex::either<int, float> floating_point { std::in_place_type<float>, 4 };
  CHECK(floating_point.index() == 1);
  CHECK(floating_point.has_other());
  CHECK(floating_point.assume_other() == 4);
}

TEST_CASE("copy-constructor") {
  apex::either<int, float> either { std::in_place_index<0>, 4 };
  apex::either<int, float> copy { either };
  CHECK(either.index() == copy.index());
  CHECK(copy.has_value());
  CHECK(copy.assume_value() == either.assume_value());
}

TEST_CASE("move-constructor") {
  apex::either<int, int> either { std::in_place_index<0>, 4 };
  apex::either<int, int> move { std::move(either) };
  CHECK(move.has_value());
  CHECK(move.index() == 0);
  CHECK(move.assume_value() == 4);
}

TEST_CASE("copy-assignment") {
  apex::either<std::string, std::string> a { std::in_place_index<0>, "Hello" };
  apex::either<std::string, std::string> b { std::in_place_index<1>, "World" };
  a = b;
  CHECK(a == b);
}

TEST_CASE("non-trivial-destructor") {
  std::string hello { "hello, world" };
  {
    apex::either<test, int> test { std::in_place_index<0>, hello };
    CHECK(std::addressof(test.value().reference) == std::addressof(hello));
  }
  CHECK(hello.empty());
}

TEST_CASE("reference-on-left") {
  int value = 4;
  apex::either<int&, float> either { std::in_place_index<0>, value };
  CHECK(either.has_value());
  CHECK(either.assume_value() == value);
  CHECK(std::addressof(either.assume_value()) == std::addressof(value));
  STATIC_REQUIRE(apex::same_as<decltype(std::move(::std::as_const(either)).assume_value()), int const&>);
  STATIC_REQUIRE(apex::same_as<decltype(std::move(either).assume_value()), int&>);
  STATIC_REQUIRE(apex::same_as<decltype(::std::as_const(either).assume_value()), int const&>);
  STATIC_REQUIRE(apex::same_as<decltype(either.assume_value()), int&>);
  STATIC_REQUIRE(apex::trivially_destructible<decltype(either)>);
}

TEST_CASE("reference-on-right") {
  int value = 4;
  apex::either<float, int&> either { std::in_place_index<1>, value };
  CHECK(either.has_other());
  CHECK(either.assume_other() == value);
  CHECK(std::addressof(either.assume_other()) == std::addressof(value));
}

TEST_CASE("reference-for-both") {
  int value = 4;
  apex::either<int&, int&> either { std::in_place_index<1>, value };
  CHECK(either.has_other());
  CHECK(either.assume_other() == value);
  CHECK(std::as_const(either).assume_other() == value);
  CHECK(std::addressof(either.assume_other()) == std::addressof(value));
}

TEST_CASE("non-trivial-destructor-with-reference-on-right") {
  int value = 4;
  apex::either<test, int&> x { std::in_place_index<1>, value };
  CHECK(::std::addressof(x.other()) == std::addressof(value));
  STATIC_REQUIRE(apex::same_as<decltype(x.other()), int&>);
  STATIC_REQUIRE(not apex::constructible_from<decltype(x), std::in_place_index_t<1>, int&&>);
}

TEST_CASE("trivial-storage-with-reference-on-right-initialized-with-reference-wrapper") {
  int value = 4;
  apex::either<int, int const&> x { std::in_place_index<1>, std::ref(value) };
  CHECK(std::addressof(x.other()) == std::addressof(value));
  CHECK(x.other() == 4);
}

TEST_CASE("complex-storage-with-reference-on-right-initialized-with-reference-wrapper") {
  int value = 4;
  apex::either<test, int const&> x { std::in_place_index<1>, std::ref(value) };
  CHECK(std::addressof(x.other()) == std::addressof(value));
  CHECK(x.other() == 4);
}

TEST_CASE("try-emplace-on-trivial-storage-with-index") {
  constexpr auto index = std::in_place_index<0>;
  apex::either<int, float> x { index, 4 };
  CHECK(x.try_emplace(index, 6) == 4);
  CHECK(::std::addressof(x.try_emplace(index, 6)) == ::std::addressof(x.assume_value()));
}

TEST_CASE("emplace-on-trivial-storage-with-index") {
  constexpr auto index = std::in_place_index<0>;
  apex::either<int, float> x { index, 4 };
  CHECK(x.emplace(index, 6) == 6);
  CHECK(::std::addressof(x.try_emplace(index, 6)) == ::std::addressof(x.assume_value()));
}
