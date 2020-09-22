#include <apex/core/either.hpp>
#include <apex/core/string.hpp>

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
  struct test {
    test(std::string& reference) noexcept : reference(reference) { }
    ~test() { this->reference.clear(); }
    std::string& reference;
  };
  std::string hello { "hello, world" };
  {
    apex::either<test, int> test { std::in_place_index<0>, hello };
    CHECK(std::addressof(test.value().reference) == std::addressof(hello));
  }
  CHECK(hello.empty());
}
