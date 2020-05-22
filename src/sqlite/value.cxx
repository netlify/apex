#include <apex/sqlite/value.hpp>
#include <sqlite3.h>

namespace apex::sqlite {

void value::swap (value& that) noexcept {
  using std::swap;
  swap(this->handle, that.handle);
}

value::operator span<byte const> () const noexcept {
  auto length = static_cast<size_t>(sqlite3_value_bytes(this->get()));
  auto data = reinterpret_cast<byte const*>(sqlite3_value_blob(this->get()));
  return span<byte const, dynamic_extent> { data, length };
}
value::operator f64 () const noexcept { return sqlite3_value_double(this->get()); }
value::operator i64 () const noexcept { return sqlite3_value_int64(this->get()); }
value::operator i32 () const noexcept { return sqlite3_value_int(this->get()); }

} /* namespace apex::sqlite */
