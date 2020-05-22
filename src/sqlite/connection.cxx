#include <apex/sqlite/connection.hpp>
#include <apex/sqlite/memory.hpp>
#include <apex/sqlite/table.hpp>
#include <apex/sqlite/error.hpp>
#include <apex/memory/std.hpp>
#include <sqlite3.h>

namespace apex::sqlite {

void plugin (connection& conn, std::string_view name, std::shared_ptr<table> item) noexcept(false) {
  auto destructor = [] (void* ptr) noexcept {
    auto pointer = static_cast<std::shared_ptr<table>*>(ptr);
    apex::destroy_at(pointer);
    deallocate(pointer);
  };
  auto db = conn.get();
  auto module = item->module();
  auto aux = ::new (allocate(sizeof(item))) std::shared_ptr<table>(item);
  if (not aux) { throw std::system_error(error::not_enough_memory); }
  auto result = sqlite3_create_module_v2(db, name.data(), module, aux, destructor);
  if (result) { throw std::system_error(error(result)); }
}

} /* namespace apex::sqlite */
