#ifndef APEX_DETAIL_PRELUDE_ENABLE_HPP
#define APEX_DETAIL_PRELUDE_ENABLE_HPP

namespace apex::detail::prelude::enable {

template <class T>
concept reference = ::std::is_lvalue_reference_v<T>;

template <class T>
concept copy_constructible = reference<T>
  or ::std::is_copy_constructible_v<T>;

template <class T>
concept move_constructible = reference<T>
  or ::std::is_move_constructible_v<T>;

template <class T>
concept copy_assignable = copy_constructible<T>
  and ::std::is_copy_assignable_v<T>;

template <class T>
concept move_assignable = move_constructible<T>
  and ::std::is_move_assignable_v<T>;

template <class... Ts>
concept copy_constructible_storage = (... and copy_constructible<Ts>);

template <class... Ts>
concept move_constructible_storage = (... and move_constructible<Ts>);

template <class... Ts>
concept copy_assignable_storage = (... and copy_assignable<Ts>);

template <class... Ts>
concept move_assignable_storage = (... and move_assignable<Ts>);

template <class... Ts>
concept constructible_storage = copy_constructible_storage<Ts...>
  and move_constructible_storage<Ts...>;

template <class... Ts>
concept assignable_storage = copy_assignable_storage<Ts...>
  and move_assignable_storage<Ts...>;

template <class...>
struct constructor {
  constructor (constructor const&) = delete;
  constructor (constructor&&) = delete;
  constructor () = default;

  constructor& operator = (constructor const&) = default;
  constructor& operator = (constructor&&) = default;
};

template <copy_constructible_storage... Ts> requires (not move_constructible_storage<Ts...>)
struct constructor<Ts...> {
  constructor (constructor const&) = default;
  constructor (constructor&&) = delete;
  constructor () = default;

  constructor& operator = (constructor const&) = default;
  constructor& operator = (constructor&&) = default;
};

template <move_constructible_storage... Ts> requires (not copy_constructible_storage<Ts...>)
struct constructor<Ts...> {
  constructor (constructor const&) = delete;
  constructor (constructor&&) = default;
  constructor () = default;

  constructor& operator = (constructor const&) = default;
  constructor& operator = (constructor&&) = default;
};

template <constructible_storage... Ts>
struct constructor<Ts...> { };

template <class... Ts>
struct assignment {
  assignment (assignment const&) = default;
  assignment (assignment&&) = default;
  assignment () = default;

  assignment& operator = (assignment const&) = delete;
  assignment& operator = (assignment&&) = delete;
};

template <copy_assignable_storage... Ts> requires (not move_assignable_storage<Ts...>)
struct assignment<Ts...> {
  assignment (assignment const&) = default;
  assignment (assignment&&) = default;
  assignment () = default;

  assignment& operator = (assignment const&) = default;
  assignment& operator = (assignment&&) = delete;
};

template <move_assignable_storage... Ts> requires (not copy_assignable_storage<Ts...>)
struct assignment<Ts...> {
  assignment (assignment const&) = default;
  assignment (assignment&&) = default;
  assignment () = default;

  assignment& operator = (assignment const&) = delete;
  assignment& operator = (assignment&&) = default;
};

template <assignable_storage... Ts>
struct assignment<Ts...> { };

} /* namespace apex::detail::prelude::enable */

#endif /* APEX_DETAIL_PRELUDE_ENABLE_HPP */
