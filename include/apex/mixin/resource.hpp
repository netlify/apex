#ifndef APEX_MIXIN_RESOURCE_HPP
#define APEX_MIXIN_RESOURCE_HPP

#include <apex/core/concepts.hpp>

namespace apex::detail::mixin::resource {

template <class T> concept element_type = requires { typename T::element_type; };
template <class T> concept pointer = requires { typename T::pointer; };

template <class T> concept element_type_only = element_type<T> and not pointer<T>;
template <class T> concept pointer_only = pointer<T> and not element_type<T>;
template <class T> concept common_type = pointer<T> and element_type<T>;

template <class> struct pointer_of;

template <element_type T>
struct pointer_of<T> :
  ::std::add_pointer<typename T::element_type>
{ };

template <pointer T>
struct pointer_of<T> :
  type_identity<typename T::pointer>
{ };

template <common_type T>
struct pointer_of<T> :
  type_identity<typename T::pointer>
{ };

template <class T>
using pointer_of_t = typename pointer_of<T>::type;

template <class T>
concept releasable = requires (T object) {
  { object.release() } -> same_as<typename pointer_of<T>::type>;
};

template <class Storage, class Pointer, class... Args>
concept resettable = requires (Storage storage, Pointer ptr, Args&&... args) {
  storage.reset(ptr, static_cast<Args&&>(args)...);
};

} /* namespace apex::detail::mixin::resource */

namespace apex::mixin {

template <class T>
concept resource_handle = requires (T object) {
  requires totally_ordered<T>;
  requires swappable<T>;
  requires movable<T>;

//  typename apex::detail::mixin::resource::pointer_of_t<T>;

  { object.get() } noexcept -> same_as<typename apex::detail::mixin::resource::pointer_of<T>::type>;
  object.operator->();
  static_cast<bool>(object);
  object.reset(object.get());
  object.reset();
};

/** @brief Provide an opt-in smart pointer like API with minimal work.
 *
 * @mixin
 *
 * This is used to save a lot of time when wrapping C APIs, of which there are
 * a great deal in the world.
 *
 * An example of intended usage:
 * ```cpp
 * template <class T, class D=std::default_delete<T>>
 * using unique_resource = handle<T, ::std::unique_ptr<T, D>>;
 * struct my_resource : protected unique_resource<int> { };
 * ```
 *
 * @note This should *only* be used for wrapping object lifetimes when other
 * types do not follow RAII. This applies to both C and C++ APIs.
 */
template <class T, resource_handle Storage>
struct resource {
  using storage_type = Storage;
  using pointer = apex::detail::mixin::resource::pointer_of_t<remove_cvref_t<storage_type>>;
  using resource_type = resource;

  template <class... Args> requires constructible_from<storage_type, pointer, Args...>
  explicit (not sizeof...(Args)) resource (pointer ptr, Args&&... args)
  noexcept(::std::is_nothrow_constructible_v<storage_type, pointer, Args...>) :
    storage { ptr, static_cast<Args&&>(args)... }
  { }

  resource (nullptr_t) noexcept :
    resource { }
  { }
  resource () noexcept = default;

  explicit operator bool () const noexcept { return static_cast<bool>(this->storage); }
  decltype(auto) get () const noexcept { return this->storage.get(); }

  template <class... Args> requires apex::detail::mixin::resource::resettable<storage_type, pointer, Args...>
  void reset (pointer ptr, Args&&... args) noexcept {
    this->storage.reset(ptr, static_cast<Args&&>(args)...);
  }

  void reset () noexcept { this->storage.reset(); }

  pointer release () noexcept requires apex::detail::mixin::resource::releasable<storage_type> {
    return this->storage.release();
  }

  friend void swap (resource& lhs, resource& rhs)
  noexcept(noexcept(::apex::ranges::swap(lhs.storage, rhs.storage))) {
    ::apex::ranges::swap(lhs.storage, rhs.storage);
  }
protected:
  storage_type storage;
};

} /* namespace apex::mixin */

#endif /* APEX_MIXIN_RESOURCE_HPP */
