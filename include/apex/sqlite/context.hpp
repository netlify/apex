#ifndef APEX_SQLITE_CONTEXT_HPP
#define APEX_SQLITE_CONTEXT_HPP

namespace apex::sqlite {

struct value;

struct context final {
  using resource_type = view_ptr<sqlite3_context>;
  using pointer = resource_type::pointer;
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

  // TODO: Use tie apex::proxy::item here instead.
  value const& operator [] (ptrdiff_t) const noexcept;

  pointer get () const noexcept;
  ptrdiff_t size () const noexcept;
  bool empty() const noexcept;
  void* user () const noexcept;

private:
  resource_type handle;
};

} /* namespace apex::sqlite */

void* operator new (std::size_t, apex::sqlite::context&);
void operator delete (void*, apex::sqlite::context&);

#endif /* APEX_SQLITE_CONTEXT_HPP */
