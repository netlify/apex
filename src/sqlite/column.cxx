#include <apex/sqlite/column.hpp>
#include <sqlite3.h>

namespace apex::sqlite {

string_view column::database () const noexcept {
  return sqlite3_column_database_name(this->get(), this->index());
}

string_view column::declared () const noexcept {
  return sqlite3_column_decltype(this->get(), this->index());
}

string_view column::origin () const noexcept {
  return sqlite3_column_origin_name(this->get(), this->index());
}

string_view column::table () const noexcept {
  return sqlite3_column_table_name(this->get(), this->index());
}

string_view column::name () const noexcept {
  return sqlite3_column_name(this->get(), this->index());
}

ptrdiff_t column::index () const noexcept { return this->idx; }
column::pointer column::get () const noexcept { return this->handle.get(); }

} /* namespace apex::sqlite */
