#ifndef APEX_MIXIN_HANDLE_HPP
#define APEX_MIXIN_HANDLE_HPP

#include <apex/core/concepts.hpp>
#include <apex/detect/operators.hpp>

#include <type_traits>

// TODO: consider renaming the type to resource_handle, or just 'resource'
// We can also use the name 'resource' for the concept itself. All of this is
// in flux.

namespace apex::mixin::impl {

template <class> struct pointer_type_of;

template <class T>
concept pointer_only = requires {
  requires not alias_element_type<T>;
  requires alias_pointer<T>;
};

template <class T>
concept element_only = requires {
  requires not alias_pointer<T>;
  requires alias_element_type<T>;
};

template <class T>
concept element_and_pointer = alias_element_type<T> and alias_pointer<T>;

template <element_and_pointer T> struct pointer_type_of<T> : type_identity<typename T::pointer> { };
template <element_only T> struct pointer_type_of<T> : add_pointer<typename T::element_type> { };
template <pointer_only T> struct pointer_type_of<T> : type_identity<typename T::pointer> { };

template <class T>
using pointer_type_of_t = typename pointer_type_of<T>::type;

template <class T>
concept handle_storage = requires (T object) {
  //requires equality_comparable_with<T, nullptr_t>;
  //requires totally_ordered<T>;
  requires movable<T>;

  // TODO: replace with better concepts later
  requires is_default_constructible_v<T>;
  requires is_swappable_v<T>;

  typename pointer_type_of_t<T>;

  //{ object.release() } noexcept -> same_as<pointer_type_of_t<T>>;
  { object.get() } noexcept -> same_as<pointer_type_of_t<T>>;
  static_cast<bool>(object);
  object.reset(object.get());
  object.reset();
};

} /* namespace apex::mixin::impl */

namespace apex::mixin {

// Intended to be used like:
/*
 * template <class T, class D=std::default_delete<T>>
 * using unique_handle = handle<T, std::unique_ptr<T, D>>;
 * struct my_type : protected unique_handle<int> { }
 */
// this saves a LOT of time and energy when wrapping C APIs, of which there are
// a great deal of in the world.
template <class T, impl::handle_storage Storage>
struct handle {
  using storage_type = Storage;
  using pointer = impl::pointer_type_of_t<remove_cvref_t<storage_type>>;
  using handle_type = handle;

  static_assert(is_detected_v<detect::pointer::arrow, storage_type>);

  handle (std::nullptr_t) noexcept : handle { } { }
  handle () noexcept = default;

  explicit operator bool () const noexcept { return bool(this->storage); }
  decltype(auto) get () const noexcept { return this->storage.get(); }

  template <class... Args>
  void reset (pointer ptr, Args&&... args) noexcept {
    this->storage.reset(ptr, std::forward<Args>(args)...);
  }
  void reset () noexcept { this->storage.reset(); }

  void swap (handle& that) noexcept {
    using std::swap;
    swap(this->storage, that.storage);
  }

  template <class... Args> requires constructible_from<storage_type, pointer, Args...>
  explicit (not sizeof...(Args)) handle (pointer ptr, Args&&... args)
    noexcept(nothrow_constructible_from<storage_type, pointer, Args...>) :
    storage { ptr, std::forward<Args>(args)... }
  { }
protected:
  storage_type storage;
};

} /* namespace apex::mixin */

#endif /* APEX_MIXIN_HANDLE_HPP */
