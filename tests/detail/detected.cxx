#include <apex/detail/prelude/detected.hpp>
#include <apex/core/traits.hpp>

template <class T> using inner_type_t = typename T::type;

TEST_CASE("detected") {
  using detected_type = apex::detected_t<inner_type_t, ::std::enable_if<true>>;
  STATIC_REQUIRE(apex::same_as<void, detected_type>);
}

TEST_CASE("detected-or") {
  using detected_type = apex::detected_or_t<int, inner_type_t, ::std::enable_if<true>>;
  using default_type = apex::detected_or_t<int, inner_type_t, ::std::enable_if<false>>;
  STATIC_REQUIRE(apex::same_as<void, detected_type>);
  STATIC_REQUIRE(apex::same_as<int, default_type>);
}
