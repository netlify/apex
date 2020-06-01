#ifndef APEX_SQLITE_COLUMN_HPP
#define APEX_SQLITE_COLUMN_HPP

#include <apex/mixin/iterator.hpp>
#include <apex/memory/view.hpp>

struct sqlite3_stmt;

namespace apex::sqlite {

using std::string_view;
struct value;

struct column final : private mixin::iterator<column> {
  using handle_type = view_ptr<sqlite3_stmt>;
  using pointer = handle_type::pointer;

  column () = delete;

  void swap (column&) noexcept;

  value const& operator [] (ptrdiff_t) const noexcept;

  ptrdiff_t distance_to (column const&) const noexcept;
  bool equals (column const&) const noexcept;
  value const& dereference () const noexcept;

  void advance (ptrdiff_t) noexcept;
  void increment () noexcept;
  void decrement () noexcept;

  pointer get () const noexcept;

  string_view database () const noexcept;
  string_view declared () const noexcept;
  string_view origin () const noexcept;
  string_view table () const noexcept;
  string_view name () const noexcept;

  bool autoincrement () const noexcept;
  bool primary_key () const noexcept;
  bool not_null () const noexcept;

  string_view collation () const noexcept;
  ptrdiff_t index () const noexcept;

private:
  handle_type handle;
  ptrdiff_t idx;
};

} /* namespace apex::sqlite */

#endif /* APEX_SQLITE_COLUMN_HPP */
