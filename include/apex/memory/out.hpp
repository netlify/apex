#ifndef APEX_MEMORY_OUT_HPPP
#define APEX_MEMORY_OUT_HPPP

#include <apex/core/functional.hpp>
#include <apex/detect/types.hpp>

// This is an implementation of inout_ptr and out_ptr
// XXX: It is still a work in progress

namespace apex::memory {
namespace impl {

template <class Resource, class Pointer, class... Args>
struct out_ptr_t {

  out_ptr_t (Resource& resource, Args... args) noexcept :
    parameters { std::forward<Args>(args)... },
    resource { std::addressof(resource) }
    ptr { traits_type::construct(resource, std::forward<Args>(args)...) }
  { }

  out_ptr_t (out_ptr_t&& that) noexcept :
    parameters { std::move(that.parameters) }
    resource { std::exchange(that.resource, nullptr) },
    ptr { std::exchange(that.ptr, pointer { }) }
  { }

  ~out_ptr () noexcept {
    if (not this->resource) { return; }
    std::apply(
      apex::bind_front(traits_type::reset<Args...>, std::ref(*this->resource), this->ptr),
      this->parameters
    );
  }

  out_ptr& operator = (out_ptr&& that) noexcept {
    this->parameters = std::move(that.parameters);
    this->resource = std::exchange(that.resource, nullptr);
    this->ptr = std::exchange(that.ptr, pointer { });
    return *this;
  }

private:
  tuple<Args...> parameters { };
  Resource* resource { };
  pointer ptr { };
}

} /* namespace impl */

template <class Pointer, class Resource>
struct out_traits {
  using pointer = detected_or_t<
    detected_or_t<Pointer, detect::types::element_type, Resource>,
    detect::types::pointer,
    Resource
  >;

  template <class... Args>
  static pointer construct (Resource& r, Args&&... args);

  template <class... Args>
  static void reset (Resource& res, pointer& ptr, Args&&... args) noexcept {
    res.reset(ptr, std::forward<Args>(args)...);
  }
};

template <class Pointer, class Resource, class... Args>
auto out_ptr (Resource& r, Args&&... args) noexcept;

template <class Resource, class... Args>
auto out_ptr (Resource& r, Args&&... args) noexcept;

} /* namespace apex::memory */

#endif /* APEX_MEMORY_OUT_HPPP */
