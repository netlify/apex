#include <apex/proxy/arrow.hpp>
#include <string>

TEST_CASE("proxy::arrow") {
  auto value = std::string { "Hello, World!" };
  apex::proxy::arrow arrow { value };
  REQUIRE(value.size() == arrow->size());
}
