#ifndef APEX_SQLITE_TABLE_HPP
#define APEX_SQLITE_TABLE_HPP

#include <apex/memory/view.hpp>
#include <apex/core/prelude.hpp>

#include <string>
#include <vector>
#include <any>

struct sqlite3_module;

namespace apex::sqlite {

using std::string;

struct connection;
struct context;

enum class restraint {
  equal_to = 2,
  greater = 4,
  less_equal = 8,
  less = 16,
  greater_equal = 32,
  match = 64,
};

enum class conflict {
  rollback = 1,
  ignore = 2,
  fail = 3,
  abort = 4, // This is the default conflict resolution
  replace = 5,
};

enum class direction : bool { ascend, descend };
enum class omit : bool { no, yes };

struct index { struct output; struct input; };
struct filter;
struct usage;

struct constraint final {
  constraint (u8, bool, i32) noexcept;

  void swap (constraint&) noexcept;

  restraint type () const noexcept;
  bool valid () const noexcept;
  i32 index () const noexcept;
private:
  restraint op;
  bool usable;
  i32 idx;
};

struct order final {
  order (u8, i32) noexcept;

  void swap (order&) noexcept;
  direction type () const noexcept;
  i32 index () const noexcept;
private:
  direction dir;
  i32 idx;
};

struct cursor {
  cursor (cursor const&) = delete;
  cursor (cursor&&) = delete;
  cursor () noexcept = default;
  virtual ~cursor () noexcept = default;

  cursor& operator = (cursor const&) = delete;
  cursor& operator = (cursor&&) = delete;

  // replace 'any' with any::random_access_range<value>
  virtual void filter (filter const&, std::any) noexcept(false) = 0;
  virtual void column (context&, i32) noexcept(false) = 0;
  virtual void next () noexcept(false) = 0;
  virtual bool ended () const noexcept = 0;
  virtual i64 row () noexcept = 0;
};

struct table {
  using function_type = auto (*)(context&) -> void;

  table (table const&) = delete;
  table (table&&) = delete;
  table () = default;
  virtual ~table () = default;

  table& operator = (table const&) = delete;
  table& operator = (table&&) = delete;

  virtual std::shared_ptr<table> clone () const noexcept(false) = 0;

  // TODO: use any::random_access_range<char const*>
  virtual void connect (std::any) noexcept(false) = 0;
  // TODO: use any::random_access_range<char const*>
  virtual void create (std::any) noexcept(false) = 0;

  virtual void disconnect () noexcept(false) = 0;
  virtual void destroy () noexcept(false) = 0;

  virtual std::shared_ptr<cursor> iterator () const noexcept(false) = 0;

  virtual index::output const& access (index::input const&) noexcept(false) = 0;

  virtual function_type find (char const*, i32) noexcept(false);
  virtual string schema () noexcept(false) = 0;
  virtual void rename (char const*) noexcept(false) = 0;

  char const* database () noexcept;
  char const* plugin () noexcept;
  char const* name () noexcept;

  void database (char const*) noexcept(false);
  void plugin (char const*) noexcept(false);
  void name (char const*) noexcept(false);

  function_type* save (function_type&&) noexcept(false);
  sqlite3_module* module () const noexcept;
private:
  std::vector<function_type> functions;

  string labels;
  string mod;
  string db;
};

struct mutator : table {
  using rowid = i64;

  virtual ~mutator () = default;

  // TODO: replace all std::any with std::any::random_access_range
  virtual void replace (std::any, rowid) noexcept(false) = 0;
  virtual void update (std::any, rowid, rowid) noexcept(false) = 0;
  virtual void insert (std::any, rowid) = 0;
  virtual void remove (std::any, rowid) = 0;

  virtual rowid generate () noexcept = 0; // called before insert
  virtual bool support_constraints () const noexcept = 0;

  conflict policy () const noexcept;
  void policy (conflict) noexcept;

private:
  view_ptr<sqlite3> db;
  conflict pol;
};

struct contract : mutator {
  virtual ~contract () = default;
  virtual void rollback () noexcept(false) = 0;
  virtual void commit () noexcept(false) = 0;
  virtual void begin () noexcept(false) = 0;
  virtual void sync () noexcept(false) = 0;
};

struct save : contract {
  using contract::rollback;
  using contract::begin;

  virtual ~save () = default;
  virtual void rollback (i32) noexcept(false) = 0;
  virtual void release (i32) noexcept(false) = 0;
  virtual void begin (i32) noexcept(false) = 0;
};

} /* namespace apex::sqlite */

#endif /* APEX_SQLITE_TABLE_HPP */
