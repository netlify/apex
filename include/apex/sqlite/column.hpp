#ifndef APEX_SQLITE_COLUMN_HPP
#define APEX_SQLITE_COLUMN_HPP

#include <apex/mixin/iterator.hpp>
#include <apex/sqlite/memory.hpp>
#include <apex/core/string.hpp>

struct sqlite3_stmt;

namespace apex::sqlite {

using std::string_view;
struct value;

// TODO: add mixin::iterator<column>
struct column final : private view_handle<sqlite3_stmt> {
  using resource_type::resource_type;
  using resource_type::get;

  column () = delete;

  friend void swap (column&, column&) noexcept;

  value const& operator [] (ptrdiff_t) const noexcept;

  ptrdiff_t distance_to (column const&) const noexcept;
  value const& read_from () const noexcept;
  void advance (ptrdiff_t) noexcept;

  zstring_view database () const noexcept;
  zstring_view declared () const noexcept;
  zstring_view origin () const noexcept;
  zstring_view table () const noexcept;
  zstring_view name () const noexcept;

  bool autoincrement () const noexcept;
  bool primary_key () const noexcept;
  bool not_null () const noexcept;

  string_view collation () const noexcept;
  ptrdiff_t index () const noexcept;

  // TODO: add casting to values

private:
  ptrdiff_t idx;
};

} /* namespace apex::sqlite */

#endif /* APEX_SQLITE_COLUMN_HPP */
