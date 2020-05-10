#include <apex/runtime/session.hpp>

namespace {

extern "C" char** environ;
char const* const* argv { };
int argc { };

[[gnu::constructor]] void initialize (int argc, char const* const* argv) {
  ::argc = argc;
  ::argv = argv;
}

} /* nameless namespace */

namespace apex::runtime {

arguments const& args () noexcept {
  static arguments instance { ::argc, ::argv };
  return instance;
}

arguments::arguments (int argc, char const* const* argv) noexcept :
  argc { argc },
  argv { argv }
{ }

bool arguments::empty () const noexcept {
  return this->size();
}

arguments::size_type arguments::size() const noexcept {
  return this->argc;
};

} /* namespace apex::runtime */
