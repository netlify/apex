#ifndef APEX_CORE_DISPATCH
#define APEX_CORE_DISPATCH

/* This is an implementation of the colloquially known tag_invoke, however
 * historically, this has been known as a tag dispatch, hence why we call this
 * `dispatch`
 */

namespace apex::detail::core::dispatch {

template <class, class...>
void dispatch () = delete;

struct function final {
  template <class CPO, class... Args>
  requires requires (CPO cpo, Args&&... args) {
    dispatch(cpo, static_cast<Args&&>(args)...);
  } constexpr decltype(auto) operator () (CPO cpo, Args&&... args) const
  noexcept(noexcept(dispatch(cpo, static_cast<Args&&>(args)...))) {
    return dispatch(cpo, static_cast<Args&&>(args)...);
  }
};

} /* namespace apex::detail::core::dispatch */

namespace apex {

inline constexpr auto const dispatch = detail::core::dispatch::function { };

template <class T, class... Args>
concept dispatchable = requires (T cpo, Args&&... args) {
  ::apex::dispatch(cpo, static_cast<Args&&>(args)...);
};

} /* namespace apex */

#endif /* APEX_CORE_DISPATCH */
