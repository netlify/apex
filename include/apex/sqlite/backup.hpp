#ifndef APEX_SQLITE_BACKUP_HPP
#define APEX_SQLITE_BACKUP_HPP

#include <apex/mixin/iterator.hpp>

#include <string_view>
#include <memory>

struct sqlite3_backup;

namespace apex::sqlite {

struct connection;

struct backup final : private mixin::iterator<backup> {
  using handle_type = std::shared_ptr<sqlite3_backup>;
  using pointer = handle_type::element_type*;

  backup (connection&, std::string_view, connection const&, std::string_view) noexcept(false);
  backup () = delete;

  backup& operator = (backup const&) = default;
  backup& operator = (backup&&) = default;

  void swap (backup&) noexcept;

  backup const& dereference () const noexcept;
  bool equals (backup const&) const noexcept;

  void advance (ptrdiff_t) noexcept;
  void increment () noexcept;

  pointer get () const noexcept;

  ptrdiff_t remaining () const noexcept;
  ptrdiff_t copied () const noexcept;
  ptrdiff_t total () const noexcept;

private:
  handle_type handle;
};

} /* namespace apex::sqlite */

#endif /* APEX_SQLITE_BACKUP_HPP */
