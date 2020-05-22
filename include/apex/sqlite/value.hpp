#ifndef APEX_SQLITE_VALUE_HPP
#define APEX_SQLITE_VALUE_HPP

#include <apex/memory/view.hpp>
#include <apex/core/span.hpp>

#include <string_view>

struct sqlite3_value;

namespace apex::sqlite {

struct column;

// TODO: make a parameter type so we can differentiate between
// unprotected/protected sqlite3_value's
struct value final {
  using handle_type = view_ptr<sqlite3_value>;
  using pointer = handle_type::pointer;

  explicit value (column const&) noexcept;
  explicit value (pointer) noexcept;

  value (value const&) = default;
  value () = delete;
  ~value () = default;

  value& operator = (value const&) = default;

  void swap (value&) noexcept;

  explicit operator std::string_view () const noexcept;
  explicit operator span<byte const> () const noexcept;
  explicit operator f64 () const noexcept;
  explicit operator i64 () const noexcept;
  explicit operator i32 () const noexcept;

  pointer get () const noexcept;

  bool operator == (value const&) noexcept;
  bool operator != (value const&) noexcept;
  bool operator >= (value const&) noexcept;
  bool operator <= (value const&) noexcept;
  bool operator > (value const&) noexcept;
  bool operator < (value const&) noexcept;

private:
  handle_type handle;
};

} /* namespace apex::sqlite */

#endif /* APEX_SQLITE_VALUE_HPP */
