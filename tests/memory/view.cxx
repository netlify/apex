#include <apex/memory/view.hpp>

TEST_CASE("view_ptr default constructor", "[view_ptr][constructor]") {
  apex::view_ptr<void> ptr { };
  REQUIRE_FALSE(ptr);
}

TEST_CASE("view_ptr nullptr constructor", "[view_ptr][constructor]") {
  apex::view_ptr<void> ptr { nullptr };
  REQUIRE_FALSE(ptr);
}

TEST_CASE("view_ptr pointer constructor", "[view_ptr][constructor]") {
  int value { 42 };
  apex::view_ptr ptr { &value };
  REQUIRE(ptr);
  REQUIRE(*ptr == value);
}

TEST_CASE("view_ptr copy constructor", "[view_ptr][constructor]") {
  int value { 42 };
  apex::view_ptr ptr { &value };
  apex::view_ptr copy { ptr };
  REQUIRE(ptr == copy);
}

TEST_CASE("view_ptr nullptr assignment", "[view_ptr][assignment]") {
  int value { 42 };
  apex::view_ptr ptr { &value };
  ptr = nullptr;
  REQUIRE_FALSE(ptr);
}

TEST_CASE("view_ptr pointer assignment", "[view_ptr][assignment]") {
  int value { 42 };
  apex::view_ptr<int> ptr { };
  ptr = &value;
  REQUIRE(ptr);
  REQUIRE(*ptr == value);
}

TEST_CASE("view_ptr copy assignment", "[view_ptr][assignment]") {
  int value { 42 };
  apex::view_ptr<int> copy { };
  apex::view_ptr ptr { &value };
  copy = ptr;
  REQUIRE(copy == ptr);
}

TEST_CASE("view_ptr static_cast", "[view_ptr][cast]") {
  int value { 42 };
  apex::view_ptr view { std::addressof(value) };
  auto ptr = static_cast<int*>(view);
  CHECK(view.get() == ptr);
}

TEST_CASE("view_ptr dereference", "[view_ptr][operators]") {
  int value { 42 };
  apex::view_ptr<int> view { std::addressof(value) };
  CHECK(42 == *view);
}

TEST_CASE("view_ptr arrow", "[view_ptr][operators]") {
  std::string const value { "arrow" };
  apex::view_ptr view { std::addressof(value) };
  CHECK(view->size() == 5);
}

TEST_CASE("view_ptr equality comparison", "[view_ptr][compare]") { }
TEST_CASE("view_ptr less than comparison", "[view_ptr][compare]") { }
