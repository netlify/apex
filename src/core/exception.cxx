#include <apex/core/exception.hpp>

namespace apex {

exception::exception (apex::source_location location) noexcept :
  location(location)
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
