#ifndef APEX_SQLITE_MEMORY_HPP
#define APEX_SQLITE_MEMORY_HPP

#include <apex/mixin/resource.hpp>
#include <apex/memory/view.hpp>
#include <memory>

namespace apex::sqlite {

template <class> struct default_delete;

void* reallocate (void*, size_t);
void* allocate (size_t);
void deallocate (void*);

size_t allocated () noexcept;

template <class T>
using view_handle = mixin::resource<
  ::std::remove_pointer_t<T>,
  view_ptr<::std::remove_pointer_t<T>>
>;

template <class T, class D=default_delete<T>>
using unique_handle = mixin::resource<T, ::std::unique_ptr<T, D>>;

} /* namespace apex::sqlite */

#endif /* APEX_SQLITE_MEMORY_HPP */
