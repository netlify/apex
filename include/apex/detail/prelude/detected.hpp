#ifndef APEX_DETAIL_PRELUDE_DETECTED_HPP
#define APEX_DETAIL_PRELUDE_DETECTED_HPP

/* The detection idiom is still useful in C++20, however at some point
 * we should sit down and modernize this to use constraints if that's
 * possible
 */
#include <type_traits>

namespace apex {

template <template <class...> class Fn, class... Args>
concept detectable_with = requires { typename Fn<Args...>; };

template <template <class...> class Fn, class... Args>
concept undetectable_with = not detectable_with<Fn, Args...>;

template <class, template <class...> class, class...> struct detected_or;
template <template <class...> class, class...> struct detected;

template <class Default, template <class...> class Fn, class... Args>
struct detected_or { using type = Default; };

template <class Default, template <class...> class Fn, class... Args>
requires detectable_with<Fn, Args...>
struct detected_or<Default, Fn, Args...> { using type = Fn<Args...>; };

template <template <class...> class Fn, class... Args>
struct detected { };

template <template <class...> class Fn, class... Args>
requires detectable_with<Fn, Args...>
struct detected<Fn, Args...> { using type = Fn<Args...>; };

template <class Default, template <class...> class Fn, class... Args>
using detected_or_t = typename detected_or<Default, Fn, Args...>::type;

template <template <class...> class Fn, class... Args>
using detected_t = typename detected<Fn, Args...>::type;

} /* namespace apex */


#endif /* APEX_DETAIL_PRELUDE_DETECTED_HPP */
