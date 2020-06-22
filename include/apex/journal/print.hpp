#ifndef APEX_JOURNAL_PRINT_HPP
#define APEX_JOURNAL_PRINT_HPP

#include <apex/journal/priority.hpp>
#include <apex/core/string.hpp>

namespace apex::journal {

template <class... Args> void print (apex::zstring_view, Args&&...);

} /* namespace apex::journal */

#endif /* APEX_JOURNAL_PRINT_HPP */
