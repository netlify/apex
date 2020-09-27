#ifndef APEX_SQLITE_BACKUP_HPP
#define APEX_SQLITE_BACKUP_HPP

#include <apex/mixin/iterator.hpp>
#include <apex/sqlite/memory.hpp>
#include <apex/core/string.hpp>

#include <memory>

struct sqlite3_backup;

namespace apex::sqlite {

struct connection;

struct backup final : private shared_handle<sqlite3_backup>/*, private mixin::iterator<backup>*/ {
  using resource_type::get;

  backup (connection&, zstring_view, connection const&, zstring_view) noexcept(false);
  backup () = delete;

  backup& operator = (backup const&) = default;
  backup& operator = (backup&&) = default;

  void swap (backup&) noexcept;

  backup const& read_from () noexcept;

  ptrdiff_t distance_to (backup const&) const noexcept;
  void next () noexcept;

  bool equals (backup const&) const noexcept;

  ptrdiff_t remaining () const noexcept;
  ptrdiff_t copied () const noexcept;
  ptrdiff_t total () const noexcept;
};

} /* namespace apex::sqlite */

#endif /* APEX_SQLITE_BACKUP_HPP */
