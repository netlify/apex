#include <apex/core/exception.hpp>

namespace apex {

exception::exception (source_location location) noexcept :
  location(location)
{ }

bad_member_access::bad_member_access (source_location location) noexcept :
  exception(location)
{ }

bad_either_access<void>::bad_either_access(source_location location) noexcept :
  bad_member_access(location)
{ }

bad_optional_access::bad_optional_access(source_location location) noexcept :
  bad_member_access(location)
{ }

char const* exception::function_name () const noexcept { return this->location.function_name(); }
char const* exception::file_name () const noexcept { return this->location.file_name(); }
size_t exception::column () const noexcept { return this->location.column(); }
size_t exception::line () const noexcept { return this->location.line(); }

apex::zstring_view exception::function () const noexcept { return this->function_name(); }
apex::zstring_view exception::file () const noexcept { return this->file_name(); }

char const* bad_either_access<void>::what () const noexcept { return "apex::bad_either_access"; }
char const* bad_optional_access::what () const noexcept { return "apex::bad_optional_access"; }

} /* namespace apex */
