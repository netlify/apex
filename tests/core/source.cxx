#include <apex/core/source.hpp>
#include <string_view>

struct structure {

  structure () = default;

  auto function_name () const noexcept { return this->location.function_name(); }
  auto file_name () const noexcept { return this->location.file_name(); }
  size_t column () const noexcept { return this->location.column(); }
  size_t line () const noexcept { return this->location.line(); }

private:
  apex::source_location location = apex::source_location::current();
};

auto function (apex::source_location l = apex::source_location::current()) {
  return l;
}

TEST_CASE("line-current") {
  REQUIRE(__LINE__ == apex::source_location::current().line());
}

TEST_CASE("line-previous") {
  constexpr auto location = apex::source_location::current();
  REQUIRE(location.line() == (__LINE__ - 1));
}

TEST_CASE("line-structure") {
  structure location { };
  REQUIRE(location.line() == 6);
}

TEST_CASE("line-function") {
  auto location = function();
  REQUIRE(location.line() == (__LINE__ - 1));
}

TEST_CASE("file-current") {
  constexpr auto location = apex::source_location::current();
  REQUIRE(location.file_name() == std::string_view(__FILE__));
}

