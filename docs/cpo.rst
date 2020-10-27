Niebloids
=========

:abbr:`CPOs (Customization Point Objects)` (sometimes referred to as
*niebloids*) are function object instances that fulfill two objectives:

  1. Unconditionally trigger type requirements on its arguments
  2. Dispatch to the correct function desired for its interface, typically with
     an :abbr:`ADL (Argument Dependent Lookup)`

The also provide the added benefit of performing a littany of checks when
implementing a concept. In this way, a :abbr:`CPO (Customization Point Object)`
is able to allow users to more easily "plug in" to an interface without having
to *always* specialize a traits type, function object, or implement a free
function.

Additionally, these :abbr:`CPOs (Customization Point Objects)` are *objects* to
postpone user provided functions that might be found during ADL.

Apex provides several these to permit implementing mixins, hooking into IO
(planned), formatting (planned), etc.

.. _iterable-niebloids:

Iterable Niebloids
------------------

Many of the :abbr:`CPOs (Customization Point Objects)` found here are used to
help implement the :struct:`apex::mixin::iterator`, though they are defined in
terms of :ref:`iterable-concepts` to more easily generate iterator categories.

These CPOs are not currently limited to *just one* interface for each type of
iterator category as this allows for greater levels of optimization as well as
permitting some iterable types to have a partially fulfilled interface.

.. namespace:: apex::iter

.. note:: The customization point objects are documented as structs, however,
          they are available as `inline constexpr` instances of the structs
          shown.

.. struct:: next

   Move some iterable to is successor. Whatever iterable is passed *must* be
   a non-:cxx:`const` lvalue reference. rvalue references will cause a compile
   time error.

   The order of operations for lookup is to first call :cxx:`.next()` on some
   object, regardless of whather it returns :cxx:`void` or not.

   .. note:: This will change at some point to require the :cxx:`.next()`
             return *either* :cxx:`void` or some :struct:`optional`.

   This is then followed by an ADL lookup form of :cxx:`next`, and lastly
   an attempt to call :struct:`advance` with :cxx:`1`.

.. struct:: prev

   .. todo:: This is a stub entry at the moment.

.. struct:: advance

   .. todo:: This is a stub entry at the moment.

.. struct:: distance_to

   .. todo:: This is a stub entry at the moment.

.. struct:: equal_to

   .. todo:: This is a stub entry at the moment.

.. struct:: read_from

   .. todo:: This is a stub entry at the moment.

.. struct:: write_into

   .. todo:: This is a stub entry at the moment.
