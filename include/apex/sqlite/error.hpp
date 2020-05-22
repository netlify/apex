#ifndef APEX_SQLITE_ERROR_HPP
#define APEX_SQLITE_ERROR_HPP

#include <system_error>

namespace apex::sqlite {

std::error_category const& category () noexcept;

// TODO: make multiple error categories for the sqlite module.
// Many are constraint violations :v
enum class error {
  none = 0, // SQLITE_OK
  generic = 1, // SQLITE_ERROR
  internal_malfunction = 2, //SQLITE_INTERNAL
  operation_not_permitted = 3, // SQLITE_PERM
  operation_aborted = 4, // SQLITE_ABORT
  resource_busy = 5, //SQLITE_BUSY
  resource_locked = 6, //SQLITE_LOCKED
  not_enough_memory = 7, //SQLITE_NO_MEM
  resource_read_only = 8, //SQLITE_READONLY
  interrupted = 9, //SQLITE_INTERRUPT
  operation_could_not_finish = 10, // SQLITE_IOERR
  resource_corrupt = 11, //SQLITE_CORRUPT
  no_such_opcode = 12, //SQLITE_NOTFOUND
  no_space_on_device = 13, //SQLITE_FULL
  cannot_open_resource = 14, //SQLITE_CANTOPEN
  protocol_error = 15, //SQLITE_PROTOCOL
  resource_schema_changed = 17, //SQLITE_SCHEMA
  argument_length_overflow = 18, //SQLITE_TOOBIG
  constraint_violated = 19, //SQLITE_CONSTRAINT
  argument_type_mismatch = 20, //SQLITE_MISMATCH
  inappropriate_operation = 21, //SQLITE_MISUSE
  no_space_in_resouce = 22, //SQLITE_NOLFS
  premission_denied = 23, //SQLITE_AUTH
  argument_out_of_range = 25, //SQLITE_RANGE
  not_a_resource = 26, //SQLITE_NOTADB

  /* extended error codes */
  operation_rolled_back = 516, // SQLITE_ABORT_ROLLBACK
  resource_busy_recovering = 261, // SQLITE_BUSY_RECOVERY
  resource_busy_snapshotting = 517, //SQLITE_BUSY_SNAPSHOT,
  cannot_open_cygwin_resource = 1038, //SQLITE_CANTOPEN_CONVPATH
  cannot_open_fullpath_resource = 782, // SQLITE_CANTOPEN_FULLPATH
  cannot_open_directory = 526, //SQLITE_CANTOPEN_ISDIR
  check_constraint_violated = 275, //SQLITE_CONSTRAINT_CHECK
  commit_constraint_violated = 531, //SQLITE_CONSTRAINT_COMMITHOOK
  foreign_key_constraint_violated = 787, //SQLITE_CONSTRAINT_FOREIGNKEY
  function_constraint_violated = 1043, //SQLITE_CONSTRAINT_FUNCTION
  not_null_constraint_violated = 1299, //SQLITE_CONSTRAINT_NOT_NULL
  primary_key_constraint_violated = 1555, //SQLITE_CONSTRAINT_PRIMARYKEY
  rowid_constraint_violated = 2579, //SQLITE_CONSTRAINT_ROWID
  trigger_constraint_violated = 1811, //SQLITE_CONSTRAINT_TRIGGER
  unique_constraint_vioalted = 2067, //SQLITE_CONSTRAINT_UNIQUE
  table_constraint_violated = 2323, //SQLITE_CONSTRAINT_VTAB
  table_corrupt = 267, //SQLITE_CORRUPT_VTAB
  filesystem_access_denied = 3338, //SQLITE_IOERR_ACCESS
  filesystem_reserved_lock = 2594, //SQLITE_IOERR_CHECKRESERVEDLOCK
};

std::error_code make_error_code (error e) {
  return std::error_code(static_cast<int>(e), category());
}

} /* namespace apex::sqlite */

namespace std {

template <> struct is_error_code_enum<apex::sqlite::error> : true_type { };

} /* namespace std */

#endif /* APEX_SQLITE_ERROR_HPP */
