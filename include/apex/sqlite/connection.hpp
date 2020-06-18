#ifndef APEX_SQLITE_CONNECTION_HPP
#define APEX_SQLITE_CONNECTION_HPP

#include <filesystem>

//#include <apex/core/outcome.hpp>

struct sqlite3;

namespace apex::sqlite {

struct context;
struct table;

enum class checkpoint { passive, full, restart, truncate };
enum class aggregated { step, final };
enum class pure : bool { no, yes };

using aggregate_t = auto (*)(context&, aggregated) -> void;
using function_t = auto (*)(context&) -> void;

struct connection {
  using handle_type = std::unique_ptr<sqlite3>;
  using pointer = handle_type::pointer;

//  static outcome<connection, std::error_code> temporary () noexcept;
//
//  static outcome<connection, std::error_code> memory (std::string const&) noexcept;
//  static outcome<connection, std::error_code> memory (char const*) noexcept;
//  static outcome<connection, std::error_code> memory () noexcept;
//
//  static outcome<connection, std::error_code> create (std::filesystem::path const&) noexcept;
//  static outcome<connection, std::error_code> edit (std::filesystem::path const&) noexcept;
//  static outcome<connection, std::error_code> open (std::filesystem::path const&) noexcept;


  void swap (connection&) noexcept;

  pointer get () const noexcept;

  ptrdiff_t changes () const noexcept;
  bool autocommit () const noexcept;
private:
  connection (pointer) noexcept;

  handle_type handle;
};

void execute (connection&, std::string_view) noexcept(false);

void aggregate (connection&, std::string_view, ptrdiff_t, pure, aggregate_t) noexcept(false);
void aggregate (connection&, std::string_view, ptrdiff_t, aggregate_t) noexcept(false);

void function (connection&, std::string_view, ptrdiff_t, pure, function_t) noexcept(false);
void function (connection&, std::string_view, ptrdiff_t, function_t) noexcept(false);

void plugin (connection&, std::string_view, std::shared_ptr<table>) noexcept(false);

void attach (connection&, std::filesystem::path const&, std::string_view) noexcept(false);
void detach (connection&, std::string_view) noexcept(false);

} /* namespace apex::sqlite */

#endif /* APEX_SQLITE_CONNECTION_HPP */
