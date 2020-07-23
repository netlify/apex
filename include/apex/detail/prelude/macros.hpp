#ifndef APEX_DETAIL_PRELUDE_MACROS_HPP
#define APEX_DETAIL_PRELUDE_MACROS_HPP

/*
 * NOTE: This is the ONLY FILE where we *do not* put headers at the very top,
 * but instead put our own macro preprocessor checks.
 * This is typically dangerous, but dangerous is our middle name compadre 😉
 */

/* clang-format off */
#if not defined(__has_cpp_attribute)
  #define __has_cpp_attribute(x) false
#endif /* not defined(__has_cpp_attribute) */

#if not defined(__has_builtin)
  #define __has_builtin(x) false
#endif /* not defined(__has_builtin) */

#define APEX_DETAIL_STRINGIFY(x) #x
#define APEX_STRINGIFY(x) APEX_DETAIL_STRINGIFY(x)

#define APEX_CONCAT(x, y) APEX_PASTE(x, y)
#define APEX_PASTE(x, y) x##y

#define APEX_CHECK_FEATURE(name) __has_feature(name)

#define APEX_CHECK_ATTRIBUTE(name) __has_cpp_attribute(name)
/* clang does *NOT* like the use of APEX_CONCAT or APEX_PASTE here */
#define APEX_CHECK_BUILTIN(name) (__has_builtin(__builtin_##name))

#define APEX_CHECK_API(name, version) (APEX_CONCAT(__cpp_lib_, name) >= APEX_CONCAT(version, L))
#define APEX_CHECK_CXX(name, version) (APEX_CONCAT(__cpp_, name) >= APEX_CONCAT(version, L))

/* These should be used sparingly, but are here for the same reason Rust permits
 * using #[allow(...)]
 */
#define APEX_DIAGNOSTIC_PRAGMA(type, flag) \
  _Pragma(APEX_STRINGIFY(clang diagnostic type APEX_STRINGIFY(APEX_CONCAT(-W, flag))))
#define APEX_DIAGNOSTIC_FORBID(diagnostic) APEX_DIAGNOSTIC_PRAGMA(fatal, diagnostic)
#define APEX_DIAGNOSTIC_ALLOW(diagnostic) APEX_DIAGNOSTIC_PRAGMA(ignored, diagnostic)
#define APEX_DIAGNOSTIC_WARN(diagnostic) APEX_DIAGNOSTIC_PRAGMA(warning, diagnostic)
#define APEX_DIAGNOSTIC_DENY(diagnostic) APEX_DIAGNOSTIC_PRAGMA(error, diagnostic)
#define APEX_DIAGNOSTIC_PUSH() _Pragma("clang diagnostic push")
#define APEX_DIAGNOSTIC_POP() _Pragma("clang diagnostic pop")

#define APEX_MESSAGE_PRAGMA(type, message) _Pragma(APEX_STRINGIFY(GCC type(message)))
#define APEX_ERROR(message) APEX_MESSAGE_PRAGMA(error, message)
#define APEX_WARN(message) APEX_MESSAGE_PRAGMA(warning, message)

#if __has_include(<version>)
  #include <version>
#endif /* __has_include(<version>) */
/* clang-format on */

#endif /* APEX_DETAIL_PRELUDE_MACROS_HPP */
