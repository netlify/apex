Mixins
======

.. namespace:: apex::mixin

.. concept:: template <class T> resource_handle

   A resource handle is effectively any existing smart pointer *or* smart
   pointer like type that handles construction and destruction (even if it is
   a non-owning smart pointer like type)

   :conjoins with:
      * :concept:`totally_ordered\<T>`
      * :concept:`swappable\<T>`
      * :concept:`movable\<T>`

   :valid expressions:
      Given a :expr:`T` named :expr:`object`

      * :expr:`object.get()` returns *either of*
          * :expr:`typename T::pointer`
          * :expr:`typename T::element_type*`
      * :expr:`object.operator->()`
      * :expr:`static_cast<bool>(object)`
      * :expr:`object.reset(object.get())`
      * :expr:`object.reset()`

.. struct:: template <class T, resource_handle Storage> resource

   This is typically used to quickly and automatically create several different
   "handle"-like types for handling memory resources. Typically, this means
   saving a lot of time when wrapping C APIs or C-like APIS (e.g., when using
   `cbindgen <https://github.com/eqrion/cbindgen>`_)

   :example:

     .. code-block:: cpp

        template <class T, class D=std::default_delete<T>>
        using unique_resource = resource<T, ::std::unique_ptr<T, D>>;
        struct my_resource : protected unique_resource<int> { };

   .. note:: This mixin should *only* be used for wrapping object lifetimes
             when other types do not follow RAII. This applies to both C and
             C++ APIs

   .. type:: pointer

      This type alias is *either* :expr:`typename storage_type::pointer` *or*
      :expr:`std::add_pointer_t<typename storage_type::element_type>`.

   .. type:: resource_type = resource

      A type alias given for child classes to easily refer to the resource
      without having to know all of the template parameters.

   .. type:: storage_type = Storage

      A brief alias to refer to the :concept:`resource_handle`

   .. member:: storage_type storage

      This member is available to all child classes.

   .. function:: resource::resource(std::nullptr_t) noexcept

      This is equivalent to default constructing the resource.

   .. function:: template <class... Args> resource::resource (pointer ptr, Args&&... args)

      :explicit: :expr:`not sizeof...(Args)`
      :noexcept: :expr:`safely_constructible_from<storage_type, pointer, Args...>`
      :requires: :expr:`constructible_from<storage_type, pointer, Args...>`

   .. function:: pointer get () noexcept

      Equivalent to calling :expr:`storage.get()`

   .. function:: pointer release () noexcept

      Equivalent to :expr:`storage.release()`

      :requires: :expr:`storage.release()` must be a valid expression

   .. function:: template <class... Args> void reset (pointer ptr, Args&&... args) noexcept

      Equivalent to calling :expr:`storage.reset(ptr, std::forward<Args>(args)...)`

      :requires: :expr:`storage.reset(ptr, std::forward<Args>(args)...)` must
                 be a valid expression

   .. function:: void reset () noexcept

      Equivalent to call :expr:`storage.reset()`.
