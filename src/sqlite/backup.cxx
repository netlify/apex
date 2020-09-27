#include <apex/sqlite/connection.hpp>
#include <apex/sqlite/backup.hpp>
#include <sqlite3.h>

namespace apex::sqlite {

backup::backup (connection& dst, zstring_view dst_name,
                connection const& src, zstring_view src_name) noexcept(false) :
  resource_type {
    sqlite3_backup_init(dst.get(), dst_name.data(), src.get(), src_name.data()),
    sqlite3_backup_finish
  }
{ }

backup const& backup::read_from () noexcept { return *this; }

//void backup::advance (ptrdiff_t n) noexcept {
//  auto result = sqlite3_backup_step(this->get(), n);
//  if (result == SQLITE_OK) { return; }
//  if (result == SQLITE_DONE) {
//    // TODO: Set sqlite3_backup into non-iterable state
//    // Alternatively, we just need to support some way of comparing backup
//    // to sqlite::{error||status}::done :)
//  }
//  // TODO: handle cases where a step has 'failed'
//}

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
