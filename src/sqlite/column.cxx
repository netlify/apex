#include <apex/sqlite/column.hpp>
#include <apex/core/prelude.hpp>
#include <sqlite3.h>

namespace apex::sqlite {

void swap (column& lhs, column& rhs) noexcept {
  ranges::swap(lhs.idx, rhs.idx);
}

ptrdiff_t column::distance_to (column const& that) const noexcept {
  return that.index() - this->index();
}

void column::advance (ptrdiff_t offset) noexcept { this->idx += offset; }

zstring_view column::database () const noexcept {
  return sqlite3_column_database_name(this->get(), this->index());
}

zstring_view column::declared () const noexcept {
  return sqlite3_column_decltype(this->get(), this->index());
}

zstring_view column::origin () const noexcept {
  return sqlite3_column_origin_name(this->get(), this->index());
}

zstring_view column::table () const noexcept {
  return sqlite3_column_table_name(this->get(), this->index());
}

zstring_view column::name () const noexcept {
  return sqlite3_column_name(this->get(), this->index());
}

ptrdiff_t column::index () const noexcept { return this->idx; }

} /* namespace apex::sqlite */
