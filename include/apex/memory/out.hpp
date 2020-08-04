#ifndef APEX_MEMORY_OUT_HPPP
#define APEX_MEMORY_OUT_HPPP

#include <apex/core/functional.hpp>
#include <apex/core/concepts.hpp>
#include <apex/core/meta.hpp>
#include <memory>
#include <tuple>

namespace apex::detail::out {

template <class T> using element_type = typename T::element_type;
template <class T> using pointer = typename T::pointer;

template <class T>
using pointer_of_t = detected_or_t<
  ::std::add_pointer_t<detected_t<element_type, ::std::pointer_traits<T>>>,
  pointer,
  T
>;

template <class, class U>
struct pointer_of_or : type_identity<U> { };

template <class T, class U>
requires detectable_with<pointer, T>
struct pointer_of_or<T, U> :
  type_identity<typename T::pointer>
{ };

template <class T, class U>
requires undetectable_with<pointer, T> and detectable_with<element_type, T>
struct pointer_of_or<T, U> :
  type_identity<::std::add_pointer_t<typename T::element_type>>
{ };

template <class T, class U>
using pointer_of_or_t = typename pointer_of_or<T, U>::type;

template <class T, class U>
concept pointer_with_fallback = requires (T x, U y) {
  typename pointer_of_or<T, U>::type;
};

template <class Smart, class Pointer, class... Args>
concept resettable = pointer_with_fallback<Smart, Pointer>
  and requires (Smart* s, Pointer p, Args&&... args) {
    s->reset(static_cast<pointer_of_or_t<Smart, Pointer>>(p), static_cast<Args&&>(args)...);
  };

template <class Smart, class Pointer, class... Args>
requires resettable<Smart, Pointer, Args...>
void reset (Smart* s, Pointer p, Args&&... args)
noexcept(noexcept(s->reset(static_cast<pointer_of_or_t<Smart, Pointer>>(p), static_cast<Args&&>(args)...))) {
  s->reset(static_cast<pointer_of_or_t<Smart, Pointer>>(p), static_cast<Args&&>(args)...);
}

template <class Smart, class Pointer, class... Args>
requires pointer_with_fallback<Smart, Pointer>
void reset (Smart* s, Pointer p, Args&&... args)
noexcept(noexcept(*s = Smart(static_cast<pointer_of_or_t<Smart, Pointer>>(p), static_cast<Args&&>(args)...))) {
  *s = Smart(static_cast<pointer_of_or_t<Smart, Pointer>>(p), static_cast<Args&&>(args)...);
}

} /* namespace apex::detail::out */

namespace apex {

// TODO: Consider moving this to the detail namespace
template <class Resource, class Pointer, class... Args>
struct out_ptr_t final {

  out_ptr_t (Resource& resource, Args... args) :
    parameters { static_cast<Args&&>(args)... },
    resource { ::std::addressof(resource) },
    ptr { }
  { }

  out_ptr_t (out_ptr_t&& that) noexcept :
    parameters { ::std::move(that.parameters) },
    resource { that.resource },
    ptr { ::std::exchange(that.ptr, Pointer { }) }
  { }

  // TODO: When possible, have this use a constraint. Under Clang 10,
  // this is not currently supported, but will eventually be thanks to C++20's
  // changes to support multiple destructors 🥳.
  // That said, it's hard to mark this as noexcept, thanks to the indirection of
  // using std::tuple. I just want language level parameter packs, is that
  // too much to ask?
  ~out_ptr_t () {
    if (not this->ptr) { return; }
    [this]<size_t... Is>(::std::index_sequence<Is...>)
    noexcept(noexcept(detail::out::reset(this->resource, this->ptr, ::std::get<Is>(this->parameters)...))) {
      detail::out::reset(this->resource, this->ptr, ::std::get<Is>(this->parameters)...);
    }(::std::index_sequence_for<Args...>{});
  }

  out_ptr_t& operator = (out_ptr_t&& that) noexcept {
    this->parameters = ::std::move(that.parameters);
    this->resource = ::std::move(that.resource);
    this->ptr = ::std::exchange(that.ptr, Pointer { });
    return *this;
  }

  operator Pointer* () const noexcept { return ::std::addressof(this->ptr); }
  operator Pointer* () noexcept { return ::std::addressof(this->ptr); }
  operator void** () const noexcept requires different_from<Pointer, void*> {
    return ::std::addressof(ptr);
  }

private:
  ::std::tuple<Args...> parameters;
  Resource* resource;
  Pointer ptr;
};

template <class Pointer, class Smart, class... Args>
out_ptr_t<Smart, Pointer, Args...> out_ptr (Smart& s, Args&&... args) {
  return out_ptr_t<Smart, Pointer, Args...>(s, static_cast<Args&&>(args)...);
}

template <class Smart, class... Args>
out_ptr_t<Smart, detail::out::pointer_of_t<Smart>, Args...> out_ptr (Smart& s, Args&&... args) {
  return out_ptr_t<Smart, detail::out::pointer_of_t<Smart>, Args...>(s, static_cast<Args&&>(args)...);
}

} /* namespace apex */

#endif /* APEX_MEMORY_OUT_HPPP */
