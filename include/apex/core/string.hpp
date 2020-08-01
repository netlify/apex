#ifndef APEX_CORE_STRING_HPP
#define APEX_CORE_STRING_HPP

#include <apex/core/concepts.hpp>
#include <string_view>
#include <iterator>
#include <limits>

#include <cstring>
#include <cctype>

namespace apex {

// TODO: Consider putting this into a .cxx file instead
// alternatively, make this templated (to support char8_t)?
struct case_insensitive_traits : std::char_traits<char> {
  static bool eq (char c1, char c2) noexcept { return convert(c1) == convert(c2); }
  static bool lt (char c1, char c2) noexcept { return convert(c1) == convert(c2); }

  static int compare (char const* s1, char const* s2, size_t n) noexcept {
    return strncasecmp(s1, s2, n);
  }

  static auto find (char const* s, size_t n, char const& a) noexcept {
    auto begin = s;
    auto end = s + n;
    auto iter = ::std::find_if(begin, end, [a = convert(a)] (auto c) noexcept {
      return convert(c) == a;
    });
    return iter != end ? iter : nullptr;
  }
private:
  static char convert (char ch) noexcept {
    return static_cast<char>(::std::tolower(static_cast<unsigned char>(ch)));
  }
};

template <class CharT, class Traits=::std::char_traits<CharT>>
struct basic_zstring_view {
  using difference_type = ptrdiff_t;
  using value_type = CharT;
  using size_type = size_t;

  using reference = value_type const&;
  using pointer = value_type const*;

  using const_reference = reference;
  using const_pointer = pointer;

  using const_iterator = pointer;
  using iterator = pointer;

  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using reverse_iterator = const_reverse_iterator;

  using traits = Traits;

  template <class Allocator>
  using string_type = std::basic_string<CharT, Traits, Allocator>;
  using view_type = std::basic_string_view<CharT, Traits>;

  static constexpr size_type npos = std::numeric_limits<size_type>::max();

  template <class Allocator>
  explicit basic_zstring_view (string_type<Allocator> const& that) noexcept :
    str { that.data() },
    len { that.size() }
  { }

  constexpr basic_zstring_view (pointer str) noexcept :
    str { str },
    len { traits::length(str) }
  { }

  constexpr basic_zstring_view (basic_zstring_view const&) noexcept = default;
  constexpr basic_zstring_view () noexcept = default;

  basic_zstring_view& operator = (basic_zstring_view const&) noexcept = default;

  void swap (basic_zstring_view& that) noexcept {
    std::swap(this->str, that.str);
    std::swap(this->len, that.len);
  }

  constexpr reference operator [] (size_type idx) const noexcept { return this->data()[idx]; }

  template <class Allocator>
  explicit operator string_type<Allocator> () const {
    return string_type<Allocator> { this->begin(), this->end() };
  }

  explicit operator view_type () const noexcept { return { this->data(), this->size() }; }

  // TODO: Replace all comparison operators with the code below.
  // template <class Allocator>
  // constexpr auto operator <=> (string_type<Allocator> const& that) const noexcept {
  //   return static_cast<<view_type>(*this) <=> that;
  // }
  // constexpr auto operator <=> (basic_zstring_view const& that) const noexcept {
  //   return *this <=> static_cast<view_type>(that);
  // }
  // constexpr auto operator <=> (view_type const& that) const noexcept {
  //   return static_cast<view_type>(*this) <=> that;
  // }
  // constexpr auto operator <=> (const_pointer that) const noexcept {
  //   return static_cast<view_type>(*this) <=> that;
  // }

  constexpr bool operator == (basic_zstring_view const& that) const noexcept {
    return this->size() == that.size() and this->compare(that) == 0;
  }

  constexpr bool operator != (basic_zstring_view const& that) const noexcept {
    return not (*this == that);
  }

  constexpr bool operator >= (basic_zstring_view const& that) const noexcept {
    return this->compare(that) >= 0;
  }

  constexpr bool operator <= (basic_zstring_view const& that) const noexcept {
    return this->compare(that) <= 0;
  }

  constexpr bool operator > (basic_zstring_view const& that) const noexcept {
    return this->compare(that) > 0;
  }

  constexpr bool operator < (basic_zstring_view const& that) const noexcept {
    return this->compare(that) < 0;
  }

  // TODO: Replace with operator <=> when possible
  template <class Allocator>
  bool operator == (string_type<Allocator> that) const noexcept {
    return this->size() == that.size() and this->compare(that) == 0;
  }

  template <class Allocator>
  bool operator != (string_type<Allocator> that) const noexcept {
    return not (*this == that);
  }

  template <class Allocator>
  bool operator >= (string_type<Allocator> that) const noexcept {
    return this->compare(that) >= 0;
  }

  template <class Allocator>
  bool operator <= (string_type<Allocator> that) const noexcept {
    return this->compare(that) <= 0;
  }

  template <class Allocator>
  bool operator > (string_type<Allocator> that) const noexcept {
    return this->compare(that) > 0;
  }

  template <class Allocator>
  bool operator < (string_type<Allocator> that) const noexcept {
    return this->compare(that) < 0;
  }

  // TODO: Replace with operator <=> when possible
  constexpr bool operator == (view_type that) const noexcept {
    return this->size() == that.size() and this->compare(that) == 0;
  }

  constexpr bool operator != (view_type that) const noexcept {
    return not (*this == that);
  }

  constexpr bool operator >= (view_type that) const noexcept {
    return this->compare(that) >= 0;
  }

  constexpr bool operator <= (view_type that) const noexcept {
    return this->compare(that) <= 0;
  }

  constexpr bool operator > (view_type that) const noexcept {
    return this->compare(that) > 0;
  }

  constexpr bool operator < (view_type that) const noexcept {
    return this->compare(that) < 0;
  }

  // TODO: Replace with operator <=> when possible
  constexpr bool operator == (const_pointer that) const noexcept {
    return *this == basic_zstring_view(that);
  }

  constexpr bool operator != (const_pointer that) const noexcept {
    return *this != basic_zstring_view(that);
  }

  constexpr bool operator >= (const_pointer that) const noexcept {
    return *this >= basic_zstring_view(that);
  }

  constexpr bool operator <= (const_pointer that) const noexcept {
    return *this <= basic_zstring_view(that);
  }

  constexpr bool operator > (const_pointer that) const noexcept {
    return *this > basic_zstring_view(that);
  }

  constexpr bool operator < (const_pointer that) const noexcept {
    return *this < basic_zstring_view(that);
  }

  constexpr const_reverse_iterator crebgin () const noexcept { return this->rbegin(); }
  constexpr const_reverse_iterator crend () const noexcept { return this->rend(); }

  constexpr const_iterator cbegin () const noexcept { return this->begin(); }
  constexpr const_iterator cend () const noexcept { return this->end(); }

  constexpr const_reverse_iterator rbegin () const noexcept {
    return const_reverse_iterator { this->end() };
  }

  constexpr const_reverse_iterator rend () const noexcept {
    return const_reverse_iterator { this->begin() - 1 };
  }

  constexpr const_iterator begin () const noexcept { return this->data(); }
  constexpr const_iterator end () const noexcept { return this->data() + this->size(); }

  constexpr size_type max_size () const noexcept { return npos; }
  constexpr size_type length () const noexcept { return this->size(); }
  constexpr size_type size () const noexcept { return this->len; }

  [[nodiscard]] constexpr bool empty () const noexcept { return not this->size(); }

  constexpr reference front () const noexcept { return this->data()[0]; }
  constexpr reference back () const noexcept { return this->data()[this->size() - 1]; }
  constexpr pointer c_str () const noexcept { return this->data(); }
  constexpr pointer data () const noexcept { return this->str; }

  void remove_prefix (size_type n) noexcept {
    if (n > this->size()) { n = this->size(); }
    this->str += n;
    this->len -= n;
  }

  void clear () noexcept {
    this->str = nullptr;
    this->len = 0;
  }

  [[nodiscard]] bool starts_with (value_type value) const noexcept {
    return not this->empty() and traits::eq(value, this->front());
  }

  [[nodiscard]] bool ends_with (value_type value) const noexcept {
    return not this->empty() and traits::eq(value, this->back());
  }

  [[nodiscard]] bool starts_with (basic_zstring_view that) const noexcept {
    return this->starts_with(view_type(that));
  }

  [[nodiscard]] bool ends_with (basic_zstring_view that) const noexcept {
    return this->ends_with(view_type(that));
  }

  [[nodiscard]] bool starts_with (view_type that) const noexcept {
    return this->size() >= that.size() and
      traits::compare(this->data(), that.data(), that.size()) == 0;
  }

  [[nodiscard]] bool ends_with (view_type that) const noexcept {
    return this->size() >= that.size() and
      traits::compare(this->end() - that.size(), that.data(), that.size()) == 0;
  }

  constexpr difference_type compare (basic_zstring_view that) const noexcept {
    return this->compare(static_cast<view_type>(that));
  }

  constexpr difference_type compare (view_type that) const noexcept {
    auto cmp = traits::compare(
      this->data(),
      that.data(),
      std::min(this->size(), that.size())
    );
    if (cmp != 0) { return cmp; }
    if (this->size() == that.size()) { return 0; }
    if (this->size() < that.size()) { return -1; }
    return 1;
  }

  // TODO: size_type find_first_not_of(...)
  // TODO: size_type find_last_not_of(...)
  // TODO: size_type find_first_of(...)
  // TODO: size_type find_last_of(...)
  // TODO: size_type find(...)
  //
  // TODO: size_type rfind(...)

private:
  pointer str { nullptr };
  size_type len { 0 };
};

using zstring_view = basic_zstring_view<char>;

using case_zstring_view = basic_zstring_view<char, case_insensitive_traits>;
using case_string_view = std::basic_string_view<char, case_insensitive_traits>;
using case_string = std::basic_string<char, case_insensitive_traits, std::allocator<char>>;

} /* namespace apex */

#endif /* APEX_CORE_STRING_HPP */
