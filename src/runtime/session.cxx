#include <apex/runtime/session.hpp>

namespace {

extern "C" char** environ;
char const* const* argv { };
int argc { };

// 0 - 100 are reserved for implementation use
[[gnu::constructor(101)]] void initialize (int argc, char const* const* argv) {
  ::argc = argc;
  ::argv = argv;
}

} /* nameless namespace */

namespace apex::runtime {

arguments const& args () noexcept {
  static arguments instance { ::argc, ::argv };
  return instance;
}

arguments::arguments (int count, char const* const* entries) noexcept :
  entries { entries },
  count { count }
{ }

bool arguments::empty () const noexcept {
  return this->size();
}

arguments::size_type arguments::size() const noexcept {
  return this->count;
}

char const* const* arguments::argv () const noexcept { return this->entries; }
int arguments::argc () const noexcept { return this->count; }

} /* namespace apex::runtime */
