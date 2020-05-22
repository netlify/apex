#ifndef APEX_SQLITE_TRANSACTION_HPP
#define APEX_SQLITE_TRANSACTION_HPP

#include <apex/memory/view.hpp>
#include <string_view>

namespace apex::sqlite {

struct connection;

enum class behavior { deferred, immediate, exclusive };

struct transaction final {
  transaction (connection&, behavior) noexcept;
  ~transaction () noexcept;

  void release () noexcept;
private:
  connection& handle;
  bool commit;
};

// TODO: savepoint should possibly instead be a function that returns an outcome
// This will make logging/error handling easier
struct savepoint final {
  savepoint (connection&, std::string_view) noexcept;
  ~savepoint () noexcept;

  void release () noexcept;

private:
  connection& handle;
  std::string_view name;
  bool commit;
};

} /* namespace apex::sqlite */

#endif /* APEX_SQLITE_TRANSACTION_HPP */
