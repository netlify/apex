#include <apex/sqlite/connection.hpp>
#include <apex/sqlite/backup.hpp>
#include <sqlite3.h>

namespace apex::sqlite {

backup::backup (connection& dst, std::string_view dst_name,
                connection const& src, std::string_view src_name) noexcept(false) :
  handle {
    sqlite3_backup_init(dst.get(), dst_name.data(), src.get(), src_name.data()),
    sqlite3_backup_finish,
  }
{ }

backup const& backup::dereference () const noexcept { return *this; }

bool backup::equals (backup const&) const noexcept { return false; }

void backup::advance (ptrdiff_t n) noexcept {
  auto result = sqlite3_backup_step(this->get(), n);
  if (result == SQLITE_OK) { return; }
  if (result == SQLITE_DONE) {
    // TODO: Set sqlite3_backup into non-iterable state
    // Alternatively, we just need to support some way of comparing backup
    // to sqlite::{error||status}::done :)
  }
  // TODO: handle cases where a step has 'failed'
}

ptrdiff_t backup::remaining () const noexcept {
  return sqlite3_backup_remaining(this->get());
}

ptrdiff_t backup::copied () const noexcept {
  return this->total() - this->remaining();
}

ptrdiff_t backup::total () const noexcept {
  return sqlite3_backup_pagecount(this->get());
}

} /* namespace apex::sqlite */
