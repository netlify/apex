#include <apex/sqlite/statement.hpp>
#include <sqlite3.h>

namespace apex::sqlite {

statement::statement () :
  handle { nullptr, sqlite3_finalize }
{ }

statement::pointer statement::get () const noexcept { return this->handle.get(); }

bool statement::is_readonly () const noexcept { return sqlite3_stmt_readonly(this->get()); }
bool statement::is_busy () const noexcept { return sqlite3_stmt_busy(this->get()); }

} /* namespace apex::sqlite */
