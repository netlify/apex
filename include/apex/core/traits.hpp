#ifndef APEX_TRAITS_HPP
#define APEX_TRAITS_HPP

#include <type_traits>

#include <apex/core/macros.hpp>
#include <apex/core/types.hpp>
#include <apex/detect/types.hpp>

namespace apex {
inline namespace v1 {

/* shim section */
#if APEX_CHECK_API(type_identity, 201806)
  using ::std::type_identity_t;
  using ::std::type_identity;
#else
  template <class T> struct type_identity { using type = T; };
  template <class T> using type_identity_t = typename type_identity<T>::type;
#endif /* APEX_CHECK_API(type_identity, 201806) */

#if APEX_CHECK_API(remove_cvref, 201711)
  using ::std::remove_cvref_t;
  using ::std::remove_cvref;
#else
  template <class T>
  struct remove_cvref : type_identity<
    std::remove_cv_t<std::remove_reference_t<T>>
  > { };
  template <class T> using remove_cvref_t = typename remove_cvref<T>::type;
#endif /* APEX_CHECK_API(remove_cvref, 201711) */

#if APEX_CHECK_API(bounded_array_traits, 201902)
  using ::std::is_unbounded_array;
  using ::std::is_bounded_array;

  using ::std::is_unbounded_array_v;
  using ::std::is_bounded_array_v;
#else
  template <class> struct is_unbounded_array : std::false_type { };
  template <class T> struct is_unbounded_array<T[]> : std::true_type { };
  template <class T> inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

  template <class> struct is_bounded_array : std::false_type { };
  template <class T, std::size_t N> struct is_bounded_array<T[N]> : std::true_type { };
  template <class T> inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;
#endif /* APEX_CHECK_API(bounded_array_traits, 201902) */

template <class T, class, template <class...> class, class...>
struct detector : type_identity<T> { using value_t = std::false_type; };

template <class T, template <class...> class U, class... Args>
struct detector<T, std::void_t<U<Args...>>, U, Args...> :
  type_identity<U<Args...>>
{ using value_t = std::true_type; };

struct undefined final {
  undefined (undefined const&) = delete;
  undefined () = delete;
  ~undefined () = delete;
  void operator = (undefined const&) = delete;
};

template <class T, template <class...> class U, class... Args>
using detected_or = detector<T, void, U, Args...>;

template <template <class...> class T, class... Args>
using is_detected = typename detected_or<undefined, T, Args...>::value_t;

template <template <class...> class T, class... Args>
using detected_t = typename detected_or<undefined, T, Args...>::type;

template <class T, template <class...> class U, class... Args>
using detected_or_t = typename detected_or<T, U, Args...>::type;

template <class To, template <class...> class T, class... Args>
using is_detected_convertible = std::is_convertible<
  detected_t<T, Args...>,
  To
>;

template <class T, template <class...> class U, class... Args>
using is_detected_exact = std::is_same<T, detected_t<U, Args...>>;

template <class> struct class_of;
template <class S, class T>
struct class_of<S T::*> : type_identity<T> { };

template <class T> using class_of_t = typename class_of<T>::type;

template <class T>
struct is_empty_base : std::conjunction<
  std::negation<std::is_final<T>>,
  std::is_empty<T>
> { };

template <class T>
inline constexpr auto is_empty_base_v = is_empty_base<T>::value;

template <class T>
struct is_complete :
  is_detected<detect::types::complete, T>
{ };

template <class T>
inline constexpr auto is_complete_v = is_complete<T>::value;

template <auto V>
inline constexpr auto constant = std::integral_constant<decltype(V), V> { };

template <class... Ts> inline constexpr auto always_false = false;

}} /* namespace apex::v1 */

#endif /* APEX_TRAITS_HPP */
