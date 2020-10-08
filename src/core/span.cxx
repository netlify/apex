#include <apex/core/span.hpp>
#include <vector>

#if not APEX_CHECK_API(span, 202002)

namespace apex {

template struct span<byte const, dynamic_extent>;
template struct span<byte, dynamic_extent>;

} /* namespace apex::v1 */

#endif /* APEX_CHECK_API(span, 202002) */
