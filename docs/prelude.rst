.. _prelude:

Prelude
=======

The :ref:`prelude <prelude>` is where all important declarations are placed.
These are the declarations and definitions that *everything* in apex relies on,
and all definitions declared are guaranteed to be available whenever *any*
header from apex is included.

API Check Macros
----------------

It is a fairly common practice to have to slowly upgrade a codebase over time,
lest it become stagnant and unable to compile on newer compiler releases. To
handle this (and be "forward compatible"), some way to check for implementation
features is needed. This is where API Check Macros are used.

.. c:macro:: APEX_CHECK_ATTRIBUTE(name)

   Wrapper macro around the builtin :expr:`__has_cpp_attribute` macro. This
   ends up reading in a more clear (and consistent) style.

.. c:macro:: APEX_CHECK_BUILTIN(name)

   Wrapper macro for compilers that support the preprocessor function
   :expr:`__has_builtin`. In cases where :expr:`__has_builtin` is *not*
   supported, all calls to :c:macro:`APEX_CHECK_BUILTIN` return :expr:`false`.

.. c:macro:: APEX_CHECK_FEATURE(name)

   Wrapper macro around the builtin :expr:`__has_feature` macro. This ends up
   reading in a more clear (and consistent) style.

.. c:macro:: APEX_CHECK_API(name, version)

   Given some identifier :expr:`name`, and some integer value :expr:`version`,
   returns whether the :expr:`__cpp_lib_` prefixed form is greater than or equal to
   :expr:`version`.

   :usage:
     .. code-block:: cpp
        :name: apex-check-api-example

        #if APEX_CHECK_API(ranges, 202002)
          using ::std::ranges::begin;
          using ::std::ranges::end;
        #else
          // The following lines are for exposition
          inline constexpr struct { ... } begin;
          inline constexpr struct { ... } end;
        #endif

.. c:macro:: APEX_CHECK_CXX(name, version)

   Much like :c:macro:`APEX_CHECK_API`, :c:macro:`APEX_CHECK_CXX` checks if a
   C++ *language feature* is available based on the name.

Diagnostic Configuration Macros
-------------------------------

While we tend to rely on build systems to enable or disable our diagnostics,
from time to time we want to always treat a diagnostic within a specific
category or (temporarily) disable it even if we know the code is safe. For
these reasons, we have the diagnostic configuration macros.

Diagnostics passed to any of the macros take what would follow a warning flag.
e.g., If a user wanted to *allow* ``-Wshadow``, they would write
:expr:`APEX_DIAGNOSTIC_ALLOW(shadow)`

.. c:macro:: APEX_DIAGNOSTIC_FORBID(diagnostic)

   When given a :expr:`diagnostic`, the diagnostic will cause a *fatal error*.
   when the compiler detects it has been violated. *Fatal errors* halt
   compilation of the translation unit and no further errors will be reported.

.. c:macro:: APEX_DIAGNOSTIC_DENY(diagnostic)

   When given a :expr:`diagnostic`, the diagnostic will cause a *non-fatal
   error* when the compiler detects it has been violated. *Non-fatal errors*
   do not halt compilation of the current translation unit.

.. c:macro:: APEX_DIAGNOSTIC_WARN(diagnostic)

   When given a :expr:`diagnostic`, the diagnostic will cause a *warning* when
   the compiler detects it has been violated. Warnings do not require that code
   halt compilation, nor are they guaranteed to be correct in *all* cases.
   Despite this, one should endeavor to fix warning diagnostics as your code
   will actually compiler faster!

.. c:macro:: APEX_DIAGNOSTIC_ALLOW(diagnostic)

   When given a :expr:`diagnostic`, the compiler will either *ignore* or
   *disable* detecting the diagnostic. This macro can be used in conjunction
   with :c:macro:`APEX_DIAGNOSTIC_PUSH` and :c:macro:`APEX_DIAGNOSTIC_POP` to
   temporarily disable diagnostics that have been confirmed to be false
   positives.

   .. danger:: DO NOT USE THIS JUST TO SILENCE A WARNING

.. c:macro:: APEX_DIAGNOSTIC_PUSH()

   Saves the current set of diagnostics onto an internal compiler diagnostics
   stack.  A user can then modify the current set of diagnostics, then call
   :c:macro:`APEX_DIAGNOSTIC_POP` to restore the set of diagnostics at the
   point that :c:macro:`APEX_DIAGNOSTIC_PUSH` was first called.

   This is mostly due to there not being a way to restore individual diagnostic
   values as the preprocessor cannot store such information

.. c:macro:: APEX_DIAGNOSTIC_POP()

   Restores the current set of diagnostics to the state stored from a previous
   call to :c:macro:`APEX_DIAGNOSTIC_PUSH`.

   .. danger::

      Do not call :c:macro:`APEX_DIAGNOSTIC_POP` if
      :c:macro:`APEX_DIAGNOSTIC_PUSH` has not been called at least once

.. c:macro:: APEX_ERROR(message)

   When called, the compiler will emit an error with the message provided by
   the user. This is useful for emitting custom error messages, however it can
   only be used as part of the preprocessor.

.. c:macro:: APEX_WARN(message)

   When called, the compiler will emit a warning with the message provided by
   the user. This is useful for emitting custom warning messages, however it
   can only be used as part of the preprocessor.

Implementation Usage Macros
---------------------------

These macros are used to detect specific information regarding an
implementation. Currently these are only used to detect libc++ and libstdc++,
as some features need to be disabled on each. At some point these macros will
be deprecated (as they'll be unused) and later removed. This will occur when
Apex moves to clang and libc++ *only* instead of clang and libstdc++.

.. c:macro:: APEX_USES_LIBSTDCXX

   When using libstdc++ as the underlying standard library, this macro
   evaluates to :expr:`1`.

.. c:macro:: APEX_USES_LIBCXX

   When using libc++ as the underlying standard library, this macro evaluates
   to :expr:`1`
