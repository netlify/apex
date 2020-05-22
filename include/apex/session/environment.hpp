#ifndef APEX_SESSION_ENVIRONMENT_HPP
#define APEX_SESSION_ENVIRONMENT_HPP

#include <string_view>
#include <string>

namespace apex::session {

struct environment final {

  struct variable final {
    operator std::string_view () const noexcept;
    variable& operator = (std::string_view);
  };

  using value_type = variable;
  using size_type = size_t;

  template <class K>
  value_type operator [] (K const&) const;

  value_type operator [] (std::string_view const&) const  noexcept;
  value_type operator [] (std::string const&) const noexcept;
  value_type operator [] (char const*) const noexcept;

  iterator begin () const noexcept;
  iterator end () const noexcept;

  bool contains (std::string_view)  const noexcept;

  [[nodiscard]] bool empty() const noexcept;
  size_type size () const noexcept;

  template <class K>
  iterator find (K const&) const noexcept;

  template <class K>
  void erase (K const&) const noexcept;
};

} /* namespace apex::session */

#endif /* APEX_SESSION_ENVIRONMENT_HPP */
