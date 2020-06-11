#include <apex/core/string.hpp>

TEST_CASE("zstring-view-default-constructor") {
  constexpr apex::zstring_view ref { };
  CHECK(ref.length() == 0u);
  CHECK(ref.size() == 0u);
  CHECK(ref.empty());
}

TEST_CASE("zstring-view-copy-constructor") {
  constexpr apex::zstring_view ref { "hello!" };

  CHECK(ref.length() == 6u);
  CHECK(ref.size() == 6u);
  CHECK_FALSE(ref.empty());
  CHECK(ref.data());

  constexpr apex::zstring_view copy { ref };

  CHECK(copy.length() == ref.length());
  CHECK(copy.size() == ref.size());
  CHECK(copy.empty() == ref.empty());
  CHECK(copy.data() == ref.data());

  CHECK(copy == ref);
}

TEST_CASE("zstring-view-pointer-constructor") {
  constexpr auto str = "pointer-constructor";
  constexpr apex::zstring_view ref { str };

  CHECK(ref.data());
  CHECK_FALSE(ref.empty());

  CHECK(ref.size() == std::strlen(str));
  CHECK(ref.data() == str); 
}
