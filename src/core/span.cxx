#include <apex/core/span.hpp>
#include <vector>

namespace apex {
inline namespace v1 {

template struct span<byte const, dynamic_extent>;
template struct span<byte, dynamic_extent>;

}} /* namespace apex::v1 */
