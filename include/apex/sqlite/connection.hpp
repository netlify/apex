#ifndef APEX_SQLITE_CONNECTION_HPP
#define APEX_SQLITE_CONNECTION_HPP

#include <filesystem>

typedef struct sqlite3 sqlite3;

namespace apex {
inline namespace v1 {

struct connection {
  outcome<connection, error_code> temporary () noexcept;
  outcome<connection, error_code> memory () noexcept;

  outcome<connection, error_code> create (std::filesystem::path const&) noexcept;
  outcome<connection, error_code> edit (std::filesystem::path const&) noexcept;
  outcome<connection, error_code> open (std::filesystem::path const&) noexcept;
};

}} /* namespace apex::v1 */

#endif /* APEX_SQLITE_CONNECTION_HPP */
