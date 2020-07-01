#ifndef APEX_SESSION_ARGUMENTS_HPP
#define APEX_SESSION_ARGUMENTS_HPP

#include <apex/core/string.hpp>

#include <string_view>
#include <iterator>

#include <cstddef>

namespace apex::session {

// TODO: Add ssize() call
// TODO: Add string_view-like operations to reduce size/slice.
struct arguments final {
  using iterator = char const* const*; // temporary for now, will eventually be a string view iterator :)
  using reverse_iterator = std::reverse_iterator<iterator>;
  using value_type = std::string_view;
  using size_type = size_t;

  arguments (int, char const* const*) noexcept;
  arguments () noexcept = delete;

  value_type operator [] (size_type) const noexcept;
  // TODO: return an outcome<value_type, errors::out_of_range> instead
  value_type at (size_type) const noexcept(false);

  reverse_iterator rbegin () const noexcept;
  reverse_iterator rend () const noexcept;

  iterator begin () const noexcept;
  iterator end () const noexcept;

  [[nodiscard]] bool empty() const noexcept;
  size_type size () const noexcept;

  char const* const* argv () const noexcept;
  int argc () const noexcept;

private:
  char const* const* entries;
  int count;
};

} /* namespace apex::session */

#endif /* APEX_SESSION_ARGUMENTS_HPP */
