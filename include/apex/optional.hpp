#ifndef APEX_OPTIONAL_HPP
#define APEX_OPTIONAL_HPP

// Like std::optional, but supports references :)
namespace apex {
inline namespace v1 {

template <class T>
struct optional final {
  template <class... Args> optional (in_place_t, Args&&... args);

  explicit operator bool () const noexcept { return this->valid; }

  decltype(auto) operator * () const noexcept { return *this->value; }
  auto operator -> () const noexcept { return std::addressof(this->value); }

  template <class... Args>
  void emplace (Args&&... args);
private:
  union { value_type value; };
  bool valid;
};

template <class T>
struct optional<T&> final {
  using storage_type = std::add_pointer_t<std::remove_reference_t<T>>;
  using value_type = T&;

  explicit operator bool () const noexcept { return this->value; }

  decltype(auto) operator * () const noexcept { return *this->value; }
  auto operator -> () const noexcept { return this->value; }

private:
  storage_type value;
};

}} /* namespace apex::v1 */

#endif /* APEX_OPTIONAL_HPP */
