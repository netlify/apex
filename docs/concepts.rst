Concepts
========

There are several concepts provided by Apex that are not found within the C++
standard library.

.. namespace:: apex

.. concept:: template <class T, class U> similar_to

   Determines whether two types are similar (that is, in our case, if
   ``std::remove_cvref_t<T>`` and ``std::remove_cvref_t<U>`` are the same
   type). This is defined in such a way that ``std::same_as`` correctly
   *subsumes* `similar_to`.

.. concept:: template <class T, class U> different_from

   Defined as the *inverse* of :any:`similar_to`.

.. concept:: template <class F, class... Args> safely_invocable

   :conjoins with: :concept:`invocable`

   .. code-block:: cpp

      requires (F&& f, Args&&... args) {
        { ::std::invoke(static_cast<F&&>(f), static_cast<Args&&>(args)...) } noexcept;
      }

.. concept:: template <class T, template <class...> class U> specialization_of

   If :texpr:`T` is a specialization of :texpr:`U`, this constraint will be satisfied

   :usage:
      .. code-block:: cpp
         :name: specialization-of-usage

         static_assert(specialization_of<optional<int>, optional>);
         static_assert(specialization_of<tuple<int>, tuple>);

.. concept:: template <class T> trivially_copyable

   Concept version of :expr:`std::is_trivially_copyable_v<T>`

Standard Concepts
-----------------

There are several concepts defined by the standard that currently have shim
implementations until they are available within the standard implementations
that Apex uses

.. concept:: template <class F, class... Args> invocable

   .. code-block:: cpp

      requires (F&& f, Args&&... args) {
        ::std::invoke(static_cast<F&&>(f), static_cast<Args&&>(args)...);
      }

.. _iterable-concepts:

Iterable Concepts
-----------------

These concepts are defined in terms of :ref:`iterable-niebloids`. They are
currently used to implement the interface for :cxx:`apex::mixin::iterator`.
(NOTE: The mixin is currently unstable and should not be relied on until its
API has been properly stabilized)

.. namespace-push:: iter

.. concept:: template <class I> indirectly_readable_from

   .. todo:: This is a stub entry

.. concept:: template <class I, class T> indirectly_writable_into

   .. todo:: This is a stub entry

.. concept:: template <class I> indirectly_addressable

   .. todo:: This is a stub entry

.. concept:: template <class I> weakly_incrementable

   .. todo:: This is a stub entry

.. concept:: template <class I> weakly_decrementable

   .. todo:: This is a stub entry

.. concept:: template <class I, class D> weakly_steppable

   .. todo:: This is a stub entry

.. concept:: template <class I, class S> distance_measurable

   .. todo:: This is a stub entry

.. concept:: template <class I, class S> equality_comparable

.. namespace-pop::
