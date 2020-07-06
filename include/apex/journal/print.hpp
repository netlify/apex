#ifndef APEX_JOURNAL_PRINT_HPP
#define APEX_JOURNAL_PRINT_HPP

#include <apex/journal/priority.hpp>
#include <apex/core/string.hpp>
#include <apex/core/source.hpp>

namespace apex::journal {

// This is a 'hack' taken from https://cor3ntin.github.io/posts/variadic/
// because we cannot have source_location as the last parameter given.
// Sadly, this *can* lead to template bloat. Some work is needed to make sure
// this won't happen in practice.
template <class... Args>
struct print final {
  print (apex::zstring_view, Args&&..., apex::source_location location = apex::source_location::current());
};

} /* namespace apex::journal */

#endif /* APEX_JOURNAL_PRINT_HPP */
