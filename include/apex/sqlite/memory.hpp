#ifndef APEX_SQLITE_MEMORY_HPP
#define APEX_SQLITE_MEMORY_HPP

namespace apex::sqlite {

void* reallocate (void*, size_t);
void* allocate (size_t);
void deallocate (void*);

size_t allocated () noexcept;

} /* namespace apex::sqlite */

#endif /* APEX_SQLITE_MEMORY_HPP */
