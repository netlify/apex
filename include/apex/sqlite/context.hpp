#ifndef APEX_SQLITE_CONTEXT_HPP
#define APEX_SQLITE_CONTEXT_HPP

namespace apex::sqlite {

struct value;

// TODO: consider storing 127 sqlite::value objects in a thread-local static
// array for access. Alternatively, return sqlite::value's by value, rather
// than by reference. There is a reason for this, and it's briefly mentioned in
// the sqlite3 documentation.

struct context final {
  using handle_type = view_ptr<sqlite3_context>;
  using pointer = handle_type::pointer;
  using iterator = value*;

  context (pointer, ptrdiff_t, sqlite3_value**) noexcept(false);
  explicit context (pointer) noexcept(false);

  context () = delete;

  void operator = (std::error_code const&) const noexcept;

  void operator = (string_view) const noexcept;
  void operator = (value const&) const noexcept;
  void operator = (span<byte>) const noexcept;
  void operator = (f64) const noexcept;
  void operator = (u64) const noexcept;
  void operator = (u32) const noexcept;
  void operator = (i64) const noexcept;
  void operator = (i32) const noexcept;

  value const& operator [] (ptrdiff_t) const noexcept;

  pointer get () const noexcept;
  ptrdiff_t size () const noexcept;
  bool empty() const noexcept;
  void* user () const noexcept;

private:
  handle_type handle;
};

} /* namespace apex::sqlite */

void* operator new (std::size_t, apex::sqlite::context&);
void operator delete (void*, apex::sqlite::context&);

#endif /* APEX_SQLITE_CONTEXT_HPP */
