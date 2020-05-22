#ifndef APEX_SQLITE_STATEMENT_HPP
#define APEX_SQLITE_STATEMENT_HPP

#include <apex/core/iterable.hpp>
#include <apex/core/types.hpp>
#include <apex/core/scope.hpp>
#include <apex/core/span.hpp>

#include <apex/sqlite/row.hpp>

#include <memory>

struct sqlite3_stmt;

namespace apex::sqlite {

struct connection;
struct value;

// TODO: use apex::mixin::handle. This will reduce implementation work needed
// TODO: add a sqlite_deleter for these APIs
struct statement {
  using handle_type = std::unique_ptr<
    sqlite3_stmt,
    std::add_lvalue_reference_t<int(sqlite3_stmt*)>
  >;
  using pointer = handle_type::pointer;
  using iterator = row;

  statement ();

  iterator begin () const noexcept;
  iterator end () const noexcept;

  bool is_readonly () const noexcept;
  bool is_busy () const noexcept;

  pointer get () const noexcept;

private:
  ptrdiff_t index (czstr) noexcept(false);
  void execute () noexcept(false);
  void clear () noexcept;

  template <class... Args, size_t... Is>
  void command (std::index_sequence<Is...>, Args const&... args) noexcept(false) {
    scope_exit clear { [this] { this->clear(); } };
    (bind(*this, Is + 1, args), ...);
    this->execute();
  }

  template <class... Args, size_t... Is>
  auto query (std::index_sequence<Is...>, Args const&... args) noexcept(false) {
    scope_exit clear { [this] { this->clear(); } };
    (bind(*this, Is + 1, args), ...);
    return iterable<iterator> { std::begin(*this), std::end(*this) };
  }

  handle_type handle;
};

void bind (statement const&, ptrdiff_t, span<byte const>) noexcept(false);
void bind (statement const&, ptrdiff_t, std::string_view) noexcept(false);
void bind (statement const&, ptrdiff_t, value const&) noexcept(false);
void bind (statement const&, ptrdiff_t, nullptr_t) noexcept(false);

void bind (statement const&, ptrdiff_t, f64) noexcept(false);
void bind (statement const&, ptrdiff_t, u64) noexcept(false);
void bind (statement const&, ptrdiff_t, u32) noexcept(false);
void bind (statement const&, ptrdiff_t, i64) noexcept(false);
void bind (statement const&, ptrdiff_t, i32) noexcept(false);

} /* namespace apex::sqlite */

#endif /* APEX_SQLITE_STATEMENT_HPP */
