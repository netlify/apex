#include <apex/core/utility.hpp>

TEST_CASE("to_underlying") {
  enum class check : bool {
    no,
    yes
  };

  REQUIRE_FALSE(apex::to_underlying(check::no));
  REQUIRE(apex::to_underlying(check::yes));
}

TEST_CASE("offset_to") {
  struct X {
    int x;
  };
  REQUIRE(apex::offset_to(&X::x) == 0);
}

TEST_CASE("overloaded") {
  auto call = apex::overloaded {
    [] (int) { return true; },
    [] (double) { return false; }
  };
  REQUIRE_FALSE(call(0.0));
  REQUIRE(call(0));
}

#if not APEX_CHECK_API(integer_comparison_functions, 202002)
TEST_CASE("cmp_less") {
  REQUIRE(apex::cmp_less(-1, 0u));
  REQUIRE(not apex::cmp_less(1, 1));
  REQUIRE(not apex::cmp_less(1u, 1u));
  REQUIRE(not apex::cmp_less(1ul, 1u));
  REQUIRE(not apex::cmp_less(1u, 1ul));
  REQUIRE(not apex::cmp_less(1l, 1));
  REQUIRE(not apex::cmp_less(1, 1l));
  REQUIRE(not apex::cmp_less(1ul, 1));
  REQUIRE(not apex::cmp_less(1, 1ul));

  REQUIRE(apex::cmp_less(1, 2));
  REQUIRE(not apex::cmp_less(1, -1));
  REQUIRE(apex::cmp_less(1u, 2u));
  REQUIRE(apex::cmp_less(1ul, 2u) );
  REQUIRE(not apex::cmp_less(2u, 1ul));
  REQUIRE(apex::cmp_less(1l, 2));
  REQUIRE(not apex::cmp_less(2, 1l));
  REQUIRE(apex::cmp_less(1ul, 2));
  REQUIRE(not apex::cmp_less(2, 1ul));

  REQUIRE(not apex::cmp_less(std::numeric_limits<apex::u8>::max(), std::numeric_limits<apex::i8>::max()));
  REQUIRE(not apex::cmp_less(std::numeric_limits<apex::u8>::max(), -1));
}

TEST_CASE("cmp_equal") {
  REQUIRE(apex::cmp_equal(1, 1));
  REQUIRE(apex::cmp_equal(1u, 1u));
  REQUIRE(apex::cmp_equal(1ul, 1u));
  REQUIRE(apex::cmp_equal(1u, 1ul));
  REQUIRE(apex::cmp_equal(1l, 1));
  REQUIRE(apex::cmp_equal(1, 1l));
  REQUIRE(apex::cmp_equal(1ul, 1));
  REQUIRE(apex::cmp_equal(1, 1ul));

  REQUIRE(not apex::cmp_equal(1, 2));
  REQUIRE(not apex::cmp_equal(1, -1));
  REQUIRE(not apex::cmp_equal(1u, 2u));
  REQUIRE(not apex::cmp_equal(1ul, 2u));
  REQUIRE(not apex::cmp_equal(2u, 1ul));
  REQUIRE(not apex::cmp_equal(1l, 2));
  REQUIRE(not apex::cmp_equal(2, 1l));
  REQUIRE(not apex::cmp_equal(2ul, 1));
  REQUIRE(not apex::cmp_equal(1, 2ul));

  REQUIRE(not apex::cmp_equal(std::numeric_limits<apex::u8>::max(), std::numeric_limits<apex::i8>::max()));
  REQUIRE(not apex::cmp_equal(std::numeric_limits<apex::u8>::max(), -1));
}

TEST_CASE("in_range") {
  REQUIRE(apex::in_range<short>(-1l));
  REQUIRE(apex::in_range<short>(1ul));
  REQUIRE(apex::in_range<short>(1));
  REQUIRE(apex::in_range<short>(1u));

  REQUIRE(not apex::in_range<apex::i16>(std::numeric_limits<apex::u16>::max()));
  REQUIRE(apex::in_range<apex::i16>(std::numeric_limits<apex::u8>::max()));
  REQUIRE(not apex::in_range<apex::u8>(-1));
  REQUIRE(apex::in_range<apex::i8>(-1));
  REQUIRE(apex::in_range<apex::u8>(std::numeric_limits<apex::u16>::min()));
  REQUIRE(apex::in_range<apex::i8>(std::numeric_limits<apex::u16>::min()));
  REQUIRE(not apex::in_range<apex::u8>(std::numeric_limits<apex::i16>::min()));
  REQUIRE(not apex::in_range<apex::i8>(std::numeric_limits<apex::i16>::min()));

  REQUIRE(apex::in_range<apex::i8>(125));
  REQUIRE(apex::in_range<apex::u8>(255));
  REQUIRE(not apex::in_range<apex::i8>(128));
  REQUIRE(not apex::in_range<unsigned char>(258));
}
#endif /* APEX_CHECK_API(integer_comparison_functions, 202002) */
