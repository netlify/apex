#ifndef APEX_SQLITE_BLOB_HPP
#define APEX_SQLITE_BLOB_HPP

struct sqlite3_blob;

namespace apex::sqlite {

// TODO: use a default_delete from within apex
struct blob final {
  using resource_type = std::unique_ptr<sqlite3_blob>;
  using pointer = resource_type::pointer;

  static blob extend (column const&) noexcept(false);
  static blob open (column const&) noexcept(false);

  void swap (blob&) noexcept;

  pointer get () const noexcept;

  void write (span<byte const> const&) noexcept(false);
  void read (span<byte>&) noexcept(false);

  // TODO: add from/whence and i64 as parameters
  void seek ();
  i64 tell ();
  i64 size () const noexcept(false);
private:
  resource_type handle;
};

} /* namespace apex::sqlite */

#endif /* APEX_SQLITE_BLOB_HPP */
