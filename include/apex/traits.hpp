#ifndef APEX_TRAITS_HPP
#define APEX_TRAITS_HPP

#include <type_traits>

#include <apex/types.hpp>

namespace apex {
inline namespace v1 {

/* shim section */
#if __cpp_lib_type_identity >= 201806
  using ::std::type_identity;
#else
  template <class T> struct type_identity { using type = T; };
#endif /* __cpp_lib_type_identity */

#if __cpp_lib_remove_cvref >= 201711
  using ::std::remove_cvref_t;
  using ::std::remove_cvref;
#else
  template <class T>
  struct remove_cvref : type_identity<
    std::remove_cv_t<std::remove_reference_t<T>>
  > { };
  template <class T> using remove_cvref_t = typename remove_cvref<T>::type;
#endif /* __cpp_lib_remove_cvref */

#if __cpp_lib_bounded_array_traits >= 201902L
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
#endif /* __cpp_lib_bounded_array_traits >= 201902L */

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
using detected_or_t = typename detected_or_t<T, U, Args...>type;

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

template <auto V>
inline constexpr auto constant = std::integral_constant<decltype(V), V> { };

namespace detect::op {
/* prefix */
template <class T> using negate = decltype(-declval<T>());
template <class T> using postincrement = decltype(declval<T>()++);
template <class T> using postdecrement = decltype(declval<T>()--);
template <class T> using increment = decltype(++declval<T>());
template <class T> using decrement = decltype(--declval<T>());

/* Comparison */
template <class T, class U> using ne = decltype(declval<T>() != declval<U>());
template <class T, class U> using eq = decltype(declval<T>() == declval<U>());
template <class T, class U> using ge = decltype(declval<T>() >= declval<U>());
template <class T, class U> using le = decltype(declval<T>() <= declval<U>());
template <class T, class U> using gt = decltype(declval<T>() > declval<U>());
template <class T, class U> using lt = decltype(declval<T>() < declval<U>());

/* Arithmetic */
template <class T, class U> using mul = decltype(declval<T>() * declval<U>());
template <class T, class U> using div = decltype(declval<T>() / declval<U>());
template <class T, class U> using mod = decltype(declval<T>() % declval<U>());
template <class T, class U> using sub = decltype(declval<T>() - declval<U>());
template <class T, class U> using add = decltype(declval<T>() + declval<U>());

/* Bitwise */
template <class T, class U> using band = decltype(declval<T>() & declval<U>());
template <class T, class U> using bxor = decltype(declval<T>() ^ declval<U>());
template <class T, class U> using bor = decltype(declval<T>() | declval<U>());
template <class T, class U> using bsl = decltype(declval<T>() << declval<U>());
template <class T, class U> using bsr = decltype(declval<T>() >> declval<U>());
template <class T> using bnot = decltype(~declval<T>())

/* Arithmethic assign */
template <class T, class U> using imul = decltype(declval<T>() *= declval<U>());
template <class T, class U> using idiv = decltype(declval<T>() /= declval<U>());
template <class T, class U> using imod = decltype(declval<T>() %= declval<U>());
template <class T, class U> using iadd = decltype(declval<T>() += declval<U>());
template <class T, class U> using isub = decltype(declval<T>() -= declval<U>());

/* Bitwise assign */
template <class T, class U> using iand = decltype(declval<T>() &= declval<U>());
template <class T, class U> using ixor = decltype(declval<T>() ^= declval<U>());
template <class T, class U> using ior = decltype(declval<T>() |= declval<U>());
template <class T, class U> using isl = decltype(declval<T>() <<= declval<U>());
template <class T, class U> using isr = decltype(declval<T>() >>= declval<U>());

/* members */
template <class T, class I> using subscript = decltype(declval<T>()[declval<I>()]);
template <class T> using dereference = decltype(*declval<T>());
template <class T> using arrow = decltype(declval<T>().operator->());
} /* namespace detect::op */


// Detection meta-functions
/* These will be obsolete under C++20. However, we're still targeting C++17 for
 * the forseeable future.
 */
namespace detect {

template <class T> using difference_type = typename T::difference_type;
template <class T> using element_type = typename T::element_type;
template <class T> using value_type = typename T::value_type;
template <class T> using size_type = typename T::size_type;

template <class T> using reference = typename T::reference;
template <class T> using pointer = typename T::pointer;

} /* namespace detect */

}} /* namespace apex::v1 */

#endif /* APEX_TRAITS_HPP */
