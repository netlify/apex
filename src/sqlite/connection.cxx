#include <apex/sqlite/connection.hpp>
#include <apex/sqlite/memory.hpp>
#include <apex/sqlite/table.hpp>
#include <apex/sqlite/error.hpp>
#include <apex/core/memory.hpp>
#include <apex/memory/out.hpp>
#include <sqlite3.h>

namespace apex::sqlite {

void default_delete<sqlite3>::operator () (sqlite3* ptr) noexcept {
  sqlite3_close_v2(ptr);
}

//connection::connection (::std::filesystem::path const& path) noexcept(false) :
//  resource_type { }
//{
//  sqlite3_open_v2(path.c_str(), out_ptr(static_cast<resource_type&>(*this)), SQLITE_OPEN_READONLY, nullptr);
//  throw ::std::runtime_error("Not yet implemented");
//}

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
