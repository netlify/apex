#include <apex/detail/prelude/end.hpp>
#include <vector>

struct wrapped {
  std::vector<int> x { 1, 2, 3, 4 };
};

decltype(auto) end (wrapped& w) { return w.x.end(); }

TEST_CASE("ranges::end member function") {
  std::vector<int> x { 1, 2, 3, 4 };
  CHECK(apex::ranges::end(x) == x.end());
}

TEST_CASE("ranges::end array access") {
  int x[4] = { 1, 2, 3, 4 };
  CHECK(apex::ranges::end(x) == (x + 4));
}

TEST_CASE("ranges::end adl lookup") {
  wrapped w;
  CHECK(apex::ranges::end(w) == w.x.end());
}
