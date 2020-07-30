#ifndef APEX_SQLITE_MEMORY_HPP
#define APEX_SQLITE_MEMORY_HPP

#include <apex/mixin/handle.hpp>
#include <apex/memory/view.hpp>

namespace apex::sqlite {

void* reallocate (void*, size_t);
void* allocate (size_t);
void deallocate (void*);

size_t allocated () noexcept;

template <class T>
using view_handle = mixin::handle<
  ::std::remove_pointer_t<T>,
  view_ptr<::std::remove_pointer_t<T>>
>;

} /* namespace apex::sqlite */

#endif /* APEX_SQLITE_MEMORY_HPP */
