Monads
======

These are a small set of types that are *monadic* in nature, even if they do
not fit the "pure and proper" definintion of the termin monad. While other
languages have so-called "true" monads, C++ has more limitations and
requirements when it comes to handling monads, as we have to worry about strong
exception guarantees, and pull-based move semantics (i.e., non-destructive
moves).

Several of the types here are already in the C++ standard. *However*, due to
years of arguments at the academic level involving personal biases, the
standard :expr:`std::optional` and :expr:`std::variant` do not support
reference parameters. Furthermore, they are lacking in some features, such as
the proposed (but not implemented) :expr:`transform`, more explicit
:cxx:`noexcept` specifications and a faster :expr:`std::visit` in the case of
:expr:`std::variant`. Lastly the :expr:`apex::outcome<S, M, F>` is an evolution
of proposed types like :expr:`std::expected`, as well as third party libraries
like Boost.LEAF_, Boost.Outcome_, and STX_, with lessons learned from each of
their designs.

Typically when interacting with these types, their member functions fall into
several categories:

  1. Unchecked
  2. Thin
  3. Wide

More detail regarding these can be found within the :ref:`contracts` chapter
of the documentation.

.. namespace:: apex

.. struct:: template <class T> optional

   .. type:: value_type = T

   :struct:`Optional <optional>` manages an *optional* value (hence the name),
   much like the type found within the C++ standard library. However, unlike
   the C++ standard library, a few additional extensions are added. These
   include functions proposed within :wg21:`P07983`, several additional
   functions in the same vein as :wg21:`P07983`, a :func:`try_emplace` member
   function, several other operations found within Rust's
   :rust:`std::option::Option`, and the ability to have an :expr:`optional<T&>`
   (albeit with a slightly reduced interface).

   Unchecked APIs require the user to verify that :expr:`has_value() == true`
   before calling them, otherwise the program can be considered to exhibit
   undefined behavior or is simply ill-formed (no diagnostic required).

   .. function:: value_type const&& operator * () const&& noexcept
                 value_type&& operator * () && noexcept

      .. note:: These are disabled if :type:`value_type` is :expr:`T&`

      :contract: Unchecked
      :returns: rvalue reference to the stored :type:`value_type`

   .. function:: value_type const& operator * () const& noexcept
                 value_type& operator * () & noexcept

      :contract: Unchecked
      :returns: lvalue reference to the stored :type:`value_type`.

   .. function:: value_type const* operator -> () const noexcept
                 value_type* operator -> () noexcept

      The arrow operator overload mimics the one found within the C++ standard
      library's :expr:`std::optional<T>`.

      :contract: Unchecked
      :returns: Address of the stored value

   .. function:: value_type const&& value () const&& noexcept(false)
                 value_type&& value () && noexcept(false)

      First checks if :expr:`has_value() == true`. If this fails,
      :struct:`apex::bad_optional_access` is thrown.

      .. note:: These are disabled if :type:`value_type` is :expr:`T&`.

      :contract: Wide
      :returns: rvalue reference to the stored :type:`value_type`.
      :throws: :struct:`apex::bad_optional_access`

   .. function:: value_type const& value () const& noexcept(false)
                 value_type& value () & noexcept(false)

      First checks if :expr:`has_value() == true`. If :cxx:`false`,
      :struct:`apex::bad_optional_access` is thrown.

      :contract: Wide
      :returns: lvalue reference to the stored :type:`value_type`.
      :throws: :struct:`apex::bad_optional_access`

   .. function:: constexpr bool has_value () const noexcept

      :contract: Thin
      :returns: Whether the :struct:`optional` has been initialized with a
                value or not

   .. function:: constexpr specialization_of<optional> and_then (invocable<value_type const&&>&& f) const&&
                 constexpr specialization_of<optional> and_then (invocable<value_type const&>&& f) const&
                 constexpr specialization_of<optional> and_then (invocable<value_type&&>&& f) &&
                 constexpr specialization_of<optional> and_then (invocable<value_type&>&& f) &

      Takes any :expr:`invocable<value_type>` (where :type:`value_type` matches
      the same qualifiers as :expr:`*this`) that returns *some kind of*
      template specialization of :struct:`optional`. If :expr:`has_value() ==
      true`, :expr:`f` is invoked with the value returned by :func:`operator*`,
      otherwise an empty :struct:`optional` is returned.


      :noexcept: :expr:`safely_invocable<decltype(f), value_type>` where
                 :type:`value_type` has the same qualifiers as :cxx:`*this`.
      :returns: :struct:`optional` whose :type:`value_type` is the result of
                invoking :expr:`f`


   .. function:: constexpr optional or_else (invocable&& f) const&&
                 constexpr optional or_else (invocable&& f) const&
                 constexpr optional or_else (invocable&& f) &&
                 constexpr optional or_else (invocable&& f) &

      When :expr:`has_value() == false`, the result of invoke :expr:`f` is used
      to construct an :struct:`optional`. If the return type of :expr:`f` is
      :expr:`void`, then an empty :expr:`optional<value_type>` is returned.

      :returns: :expr:`*this` if :expr:`has_value() == true` else the result of
                invoking :expr:`f`
      :noexcept: :expr:`safely_invocable<decltype(f)>`

   .. function:: constexpr auto transform (invocable<value_type const&&>&& f) const&&
                 constexpr auto transform (invocable<value_type const&>&& f) const&
                 constexpr auto transform (invocable<value_type&&>&& f) &&
                 constexpr auto transform (invocable<value_type&>&& f) &

      :noexcept: :expr:`safely_invocable<decltype(f), value_type>`
      :returns: :struct:`optional` whose :type:`value_type` is the result of invoking
                :expr:`f`.

      :usage:
        .. code-block:: cpp
           :name: transform-usage
 
           apex::optional<int> optional { 42 };
           [[maybe_unused]] auto result = optional.transform([] (int i) {
             return i + i;
           });
           assert(result.value() == 84);

   .. function:: constexpr auto transform_or (U&& dv, invocable<value_type const&&>&& f) const&&
                 constexpr auto transform_or (U&& dv, invocable<value_type const&>&& f) const&
                 constexpr auto transform_or (U&& dv, invocable<value_type&&>&& f) &&
                 constexpr auto transform_or (U&& dv, invocable<value_type&>&& f) &

      .. todo:: Create a proper function signature explanation

      :parameter dv: Value used as a return when :expr:`has_value() == false`.
      :constraint: :texpr:`U&&` must be
                   :expr:`convertible_to<invoke_result<decltype(f), value_type>>`
      :noexcept: :expr:`safely_invocable<value_type>`
      :return: :expr:`optional<invoke_result_t<decltype(f), value_type>>`
               (where :type:`value_type` has the same matching qualifiers as
               :expr:`*this`) if :expr:`has_value() == true`, or
               :expr:static_cast<invoke_result_t<decltype(f), value_type>>(dv)`
               when :expr:`has_value() == false`.

   .. function:: constexpr auto transform_or_else (invocable&& df, invocable<value_type const&&>&& f) const&&
                 constexpr auto transform_or_else (invocable&& df, invocable<value_type const&>&& f) const&
                 constexpr auto transform_or_else (invocable&& df, invocable<value_type&&>&& f) &&
                 constexpr auto transform_or_else (invocable&& df, invocable<value_type&>&& f) &

       .. todo:: Create a proper function signature explanation

      :parameter default_function: Function whose return type is used when
                                   :expr:`has_value() == false`.
      :return: :expr:`optional<invoke_result_t<decltype(f), value_type>>` if
               :expr:`has_value() == true`, or
               :expr:`optional<invoke_result_t<decltype(df)>>` when
               :expr:`has_value() == false`.



.. struct:: template <class A, class B> either

   .. todo:: This is a stub and needs to be filled out with a proper interface

.. struct:: template <class S, class M, class F> outcome

   .. todo:: This is a stub and needs to be filled out with a proper interface

.. _Boost.Outcome: https://boostorg.github.io/outcome/
.. _Boost.LEAF: https://boostorg.github.io/leaf/
.. _STX: https://lamarrr.github.io/STX
