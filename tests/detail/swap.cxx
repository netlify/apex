#include <apex/detail/prelude/swap.hpp>
#include <initializer_list>
#include <algorithm>

struct swappable {
  swappable (swappable const&) = delete;
  swappable (swappable&&) = delete;
  swappable () = delete;

  swappable& operator = (swappable const&) = delete;
  swappable& operator = (swappable&&) = delete;

  explicit swappable (int value) noexcept : value { value } { }
  auto get () const noexcept { return this->value; }
private:
  friend void swap (swappable& lhs, swappable& rhs) {
    apex::ranges::swap(lhs.value, rhs.value);
  }
  int value;
};

struct movable {
  movable (movable const&) = delete;
  movable (movable&&) = default;
  movable () = delete;

  movable& operator = (movable const&) = delete;
  movable& operator = (movable&&) = default;

  explicit movable (int value) noexcept : value { value } { }
  auto get () const noexcept { return this->value; }
private:
  int value;
};

TEST_CASE("ranges::swap hidden friend only") {
  swappable x { 42 };
  swappable y { 69 };
  apex::ranges::swap(x, y);
  CHECK(x.get() == 69);
  CHECK(y.get() == 42);
}

TEST_CASE("ranges::swap movable only") {
  movable x { 42 };
  movable y { 69 };
  apex::ranges::swap(x, y);
  CHECK(x.get() == 69);
  CHECK(y.get() == 42);
}

TEST_CASE("ranges::swap array") {
  int x[10] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  int y[10] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
  apex::ranges::swap(x, y);
  std::initializer_list<int> a { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  std::initializer_list<int> b { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
  CHECK(std::equal(std::begin(y), std::end(y), std::begin(a), std::end(a)));
  CHECK(std::equal(std::begin(x), std::end(x), std::begin(b), std::end(b)));
}
