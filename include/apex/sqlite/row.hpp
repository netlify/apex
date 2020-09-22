#ifndef APEX_SQLITE_ROW_HPP
#define APEX_SQLITE_ROW_HPP

#include <apex/core/concepts.hpp>
#include <apex/memory/view.hpp>

#include <apex/sqlite/column.hpp>

struct sqlite3_stmt;

namespace apex::sqlite {

struct statement;

struct row final /*: private mixin::iterator<row>*/ {
  using resource_type = view_ptr<sqlite3_stmt>;
  using pointer = resource_type::pointer;

  explicit row (statement const&) noexcept;

  //row () = delete;

  void swap (row&) noexcept;

  column begin () const;
  column end () const;

  ptrdiff_t distance_to (row const&) const noexcept;
  column read_from () const noexcept;
  void advance (ptrdiff_t) noexcept;

private:
  resource_type handle;
  i64 count;
};

} /* namespace apex::sqlite */

#endif /* APEX_SQLITE_ROW_HPP */
