#include <apex/sqlite/transaction.hpp>
#include <apex/sqlite/connection.hpp>
#include <apex/sqlite/statement.hpp>

#include <apex/memory/std.hpp>

#include <apex/core/scope.hpp>
#include <apex/core/bit.hpp>

#include <sqlite3.h>

namespace {

using apex::sqlite::behavior;
using apex::unreachable;

auto mode (behavior b) noexcept {
  switch (b) {
    case behavior::immediate: return "BEGIN IMMEDIATE";
    case behavior::exclusive: return "BEGIN EXCLUSIVE";
    case behavior::deferred: return "BEGIN DEFERRED";
  }
  unreachable();
}

} /* nameless namespace */

namespace apex::sqlite {

transaction::transaction (connection& handle, behavior b) noexcept :
  handle { handle },
  commit { true }
{ execute(this->handle, ::mode(b)); }

transaction::~transaction () noexcept {
  auto text = this->commit ? "COMMIT" : "ROLLBACK";
  execute(this->handle, text);
}

void transaction::release () noexcept { this->commit = false; }

savepoint::savepoint (connection& handle, std::string_view name) noexcept :
  handle { handle },
  name { name },
  commit { true }
{
  auto const size = static_cast<int>(this->name.size());
  auto const data = this->name.data();
  auto text = sqlite3_mprintf("SAVEPOINT %.*Q", size, data);
  scope_exit free { [=] { sqlite3_free(text); } };
  try { execute(this->handle, text); }
  catch (...) { /* TODO: log the error */ }
}

savepoint::~savepoint () noexcept {
  static constexpr auto rollback = R"(ROLLBACK TRANSACTION TO SAVEPOINT %.*Q)";
  static constexpr auto release = R"(RELEASE SAVEPOINT %.Q)";
  auto const fmt = this->commit ? release : rollback;
  auto const size = static_cast<int>(this->name.size());
  auto const data = this->name.data();
  auto text = sqlite3_mprintf(fmt, size, data);
  scope_exit free { [=] { sqlite3_free(text); } };
  try { execute(this->handle, text); }
  catch (...) { /* TODO: log the error */ }
}

void savepoint::release () noexcept { this->commit = false; }

} /* namespace apex::sqlite */
