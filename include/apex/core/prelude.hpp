#ifndef APEX_CORE_PRELUDE_HPP
#define APEX_CORE_PRELUDE_HPP

/*
 * The prelude can most easily be described as a set of declarations and
 * definitions that are required to be defined *everywhere* for the most basic
 * of operations.
 *
 * At this time, we do not have full access to C++20 library features. As a
 * result, we need to implement some of these ourselves. Because of the
 * complexity involved with implementing these 'shim' declarations, we need to
 * place them into their own headers elsewhere to make it manageable.
 * These can be found in `apex/detail/prelude/`. As features become more
 * available, we'll be able to delete these detail headers and move the
 * non-standard declarations back to this location.
 *
 * Of course, a lot of this goes away with modules, but that is a long ways
 * away, both in terms of standard support as well as support from CMake and
 * other build systems.
 */
#include <apex/detail/prelude/macros.hpp>
#include <apex/detail/prelude/traits.hpp>
#include <apex/detail/prelude/types.hpp>

#include <apex/detail/prelude/iterator.hpp>
#include <apex/detail/prelude/concepts.hpp>
#include <apex/detail/prelude/swap.hpp>

#include <apex/detail/prelude/convertible-to.hpp>

/** @def APEX_CHECK_BUILTIN(name)
 * @brief Checks if `__builtin_<name>` is a valid identifier
 * In practice this isn't really needed as apex currently only target's one
 * release of clang.
 */

/** @def APEX_CHECK_API(name, version)
 * @brief Checks if `__cpp_lib_<name>` is greater than or equal to @p version
 * Primarily used to see if apex needs to provide its own implementation of
 * C++ standard library features.
 */

/** @def APEX_CHECK_CXX(name, version)
 * @brief Checks if `__cpp_<name>` is greater than or equal to @p version
 * Primarily used to see if apex has access to C++ language features.
 */

/** @def APEX_ERROR(message)
 * @brief Prints an error with the contents of @p message
 */

/** @def APEX_WARN(message)
 * @brief Prints a warning with the contents of @p message
 */

/** @def APEX_DIAGNOSTIC_FORBID(diagnostic)
 * @brief The compiler will now treat @p diagnostic as a fatal error
 */

/** @def APEX_DIAGNOSTIC_DENY(diagnostic)
 * @brief The compiler will now treat @p diagnostic as an error
 */

/** @def APEX_DIAGNOSTIC_WARN(diagnostic)
 * @brief The compiler will now treat @p diagnostic as a warning
 */

/** @def APEX_DIAGNOSTIC_ALLOW(diagnostic)
 * @brief The compiler will now *ignore* @p diagnostic
 */

#endif /* APEX_CORE_PRELUDE_HPP */
