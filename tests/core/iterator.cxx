#include <apex/core/iterator.hpp>
#include <algorithm>
#include <vector>

TEST_CASE("back_emplacer") {
  std::vector<int> x { 1, 2, 3, 4 };
  std::vector<int> y;
  std::transform(x.begin(), x.end(), apex::back_emplacer(y), [] (auto v) { return v + 1; });
  REQUIRE(y.size() == x.size());
  REQUIRE(x[0] == (y[0] - 1));
  REQUIRE(x[1] == (y[1] - 1));
  REQUIRE(x[2] == (y[2] - 1));
  REQUIRE(x[3] == (y[3] - 1));
}
