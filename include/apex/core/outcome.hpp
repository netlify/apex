#ifndef APEX_CORE_OUTCOME_HPP
#define APEX_CORE_OUTCOME_HPP

#include <apex/core/concepts.hpp>
#include <utility>

namespace apex {

// TODO: This needs a once over to make everything noexcept-clean
// TODO: There *does* need to be a specialization for references, because
// we can currently construct an outcome<int&, std::string> with an rvalue
// TODO: do we want to permit things like outcome<int, int>?
// TODO: Change 'state' to 'valid', because a 'valid outcome' is a success
//       and an 'invalid outcome' is an error
template <class T, class E>
struct outcome final {

  using success_type = conditional_t<
    is_lvalue_reference_v<T>,
    std::reference_wrapper<T>,
    T
  >;

  using failure_type = conditional_t<
    is_lvalue_reference_v<E>,
    std::reference_wrapper<E>,
    E
  >;

  using value_type = T;
  using error_type = E;

  template <class... Args> requires constructible_from<success_type, Args...>
  outcome (std::in_place_type_t<value_type>, Args&&... args) :
    success { std::forward<Args>(args)... },
    state { true }
  { }

  template <class... Args> requires constructible_from<error_type, Args...>
  outcome (std::in_place_type_t<error_type>, Args&&... args) :
    failure { std::forward<Args>(args)... },
    state { false }
  { }

  outcome (value_type const& value) :
    outcome { std::in_place_type<value_type>, value }
  { }

  outcome (value_type&& value) :
    outcome { std::in_place_type<value_type>, std::move(value) }
  { }

  outcome (error_type const& error) :
    outcome { std::in_place_type<error_type>, error }
  { }

  outcome (error_type&& error) :
    outcome { std::in_place_type<error_type>, std::move(error) }
  { }

  outcome (outcome const& that) :
    state { that.state }
  {
    if (*this) { construct_at(std::addressof(this->success), that.success); }
    else { construct_at(std::addressof(this->failure), that.failure); }
  }

  outcome (outcome&& that) :
    state { that.state }
  {
    if (*this) { construct_at(std::addressof(this->success), std::move(that.success)); }
    else { construct_at(std::addressof(this->failure), std::move(that.failure)); }
  }

  outcome () = delete;
  ~outcome () { this->clear(); }

  outcome& operator = (outcome const& that) {
    if (that) { this->emplace(that.success); }
    else { this->displace(that.failure); }
    return *this;
  }

  outcome& operator = (outcome&& that) {
    if (that) { this->emplace(std::move(that.success)); }
    else { this->displace(std::move(that.failure)); }
    return *this;
  }

  outcome& operator = (value_type const& value) {
    this->emplace(value);
    return *this;
  }

  outcome& operator = (value_type&& value) {
    this->emplace(std::move(value));
    return *this;
  }

  outcome& operator = (error_type const& error) {
    this->displace(error);
    return *this;
  }

  outcome& operator = (error_type&& error) {
    this->displace(std::move(error));
    return *this;
  }

  void swap (outcome& that) noexcept {
    using std::swap;
    if (not *this and not that) { return swap(this->failure, that.failure); }
    if (*this and that) { return swap(this->success, that.success); }
    auto temp = std::move(*this);
    *this = std::move(that);
    that = std::move(temp);
  }

  explicit operator bool () const noexcept { return this->valid; }

  value_type const& operator * () const noexcept { return this->success; }
  value_type& operator * () noexcept { return this->success; }

  value_type const* operator -> () const noexcept { return std::addressof(**this); }
  value_type& operator -> () noexcept { return std::addressof(**this); }

  error_type const& error () const noexcept { return this->failure; }
  error_type& error () noexcept { return this->failure; }

  template <class... Args>
  void emplace (Args&&... args) {
    this->clear();
    construct_at(std::addressof(this->success), std::forward<Args>(args)...);
    this->valid = true;
  }

  template <class... Args>
  void displace (Args&&... args) {
    this->clear();
    construct_at(std::addressof(this->failure), std::forward<Args>(args)...);
    this->valid = false;
  }

private:
  void clear () noexcept {
    if (*this) { return destroy_at(this->success); }
    destroy_at(this->failure);
  }

  union {
    success_type success;
    failure_type failure;
  };
  bool state;
};

} /* namespace apex */

#endif /* APEX_CORE_OUTCOME_HPP */
