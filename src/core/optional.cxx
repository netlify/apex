#include <apex/core/optional.hpp>
namespace apex {

template struct optional<unsigned long long>;
template struct optional<unsigned long>;
template struct optional<unsigned int>;
template struct optional<unsigned short>;

template struct optional<signed long long>;
template struct optional<signed long>;
template struct optional<signed int>;
template struct optional<signed short>;

template struct optional<unsigned char>;
template struct optional<signed char>;
template struct optional<char>;

template struct optional<double>;
template struct optional<float>;

} /* namespace apex */
