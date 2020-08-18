#ifndef APEX_DETAIL_PRELUDE_ENABLE_HPP
#define APEX_DETAIL_PRELUDE_ENABLE_HPP

namespace apex::detail::prelude::enable {

template <bool CopyConstructible, bool MoveConstructible>
struct constructor { };

template <bool CopyAssignable, bool MoveAssignable>
struct assignment { };

template <>
struct constructor<false, false> {
  constructor (constructor const&) = delete;
  constructor (constructor&&) = delete;
  constructor () = default;

  constructor& operator = (constructor const&) = default;
  constructor& operator = (constructor&&) = default;
};

template <>
struct constructor<true, false> {
  constructor (constructor const&) = default;
  constructor (constructor&&) = delete;
  constructor () = default;

  constructor& operator = (constructor const&) = default;
  constructor& operator = (constructor&&) = default;
};

template <>
struct constructor<false, true> {
  constructor (constructor const&) = delete;
  constructor (constructor&&) = default;
  constructor () = default;

  constructor& operator = (constructor const&) = default;
  constructor& operator = (constructor&&) = default;
};

template <>
struct assignment<false, false> {
  assignment (assignment const&) = default;
  assignment (assignment&&) = default;
  assignment () = default;

  assignment& operator = (assignment const&) = delete;
  assignment& operator = (assignment&&) = delete;
};

template <>
struct assignment<true, false> {
  assignment (assignment const&) = default;
  assignment (assignment&&) = default;
  assignment () = default;

  assignment& operator = (assignment const&) = default;
  assignment& operator = (assignment&&) = delete;
};

template <>
struct assignment<false, true> {
  assignment (assignment const&) = default;
  assignment (assignment&&) = default;
  assignment () = default;

  assignment& operator = (assignment const&) = delete;
  assignment& operator = (assignment&&) = default;
};

} /* namespace apex::detail::prelude::enable */

#endif /* APEX_DETAIL_PRELUDE_ENABLE_HPP */
