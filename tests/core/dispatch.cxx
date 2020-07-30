#include <apex/core/dispatch.hpp>

inline constexpr struct cpo_t final {
  template <class... Args> requires apex::dispatchable<cpo_t, Args...>
  constexpr decltype(auto) operator () (Args&&... args) const
  noexcept(noexcept(::apex::dispatch(*this, static_cast<Args&&>(args)...))) {
    return ::apex::dispatch(*this, static_cast<Args&&>(args)...);
  }
} cpo;

template <class T>
struct niebloid {
  using cpo = niebloid;
  template <class... Args> requires apex::dispatchable<T, Args...>
  constexpr decltype(auto) operator () (Args&&... args) const
  noexcept(noexcept(::apex::dispatch(*static_cast<T*>(this), static_cast<Args&&>(args)...))) {
    return ::apex::dispatch(*static_cast<T*>(this), static_cast<Args&&>(args)...);
  }
};

inline constexpr struct descendant_t :
  private niebloid<descendant_t> 
{ using cpo::operator (); } descendant;

struct dispatched {
  friend constexpr auto dispatch (descendant_t, dispatched) noexcept { return 42; }
  friend constexpr auto dispatch (cpo_t, dispatched) noexcept { return true; }
};

TEST_CASE("dispatch") {
  CHECK(::apex::dispatch(cpo, dispatched { }));
}

TEST_CASE("crtp-dispatch") {
  CHECK(::apex::dispatch(descendant, dispatched { }) == 42);
}
