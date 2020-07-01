#ifndef APEX_CORE_ITERABLE_HPP
#define APEX_CORE_ITERABLE_HPP

#include <iterator>

// Allows us to turn any 'range' into some iterable object. This differs from
// C++20 ranges, but it's meant to be here as a temporary workaround until
// we actually have access to them. That said, this has a bit of a different
// API than existing range-like interfaces. Effectively, every iterable<I, S>
// is itself an iterator.
// TODO: Remove this, as we can rely on ranges instead.

namespace apex {

// TODO: add detection calls for std::begin|std::end/T.begin()|T.end()
template <class I, class S=I>
struct iterable final {
  using iterator = I;
  using sentinel = S;

  template <class T, class U>
  iterable (T&& start, U&& stop) :
    start { std::forward<T>(start) },
    stop { std::forward<T>(stop) }
  { }

  // make conditionally noexcept!
  void swap (iterable& that) noexcept {
    using std::swap;
    swap(this->start, that.start);
    swap(this->stop, that.stop);
  }

  explicit operator bool () const noexcept { return this->start != this->stop; }
  decltype(auto) operator * () const noexcept { return *this->start; }
  auto operator -> () const noexcept { return std::addressof(**this); }

  iterable operator ++ (int) { return iterable(this->start++, this->stop); }
  iterable& operator ++ () noexcept { return ++this->start; return *this; }

  iterator begin () const noexcept { return this->start; }
  sentinel end () const noexcept { return this->stop; }

  auto reverse () const noexcept {
    return iterable {
      std::make_reverse_iterator(this->end()--),
      std::make_reverse_iterator(this->begin()--)
    };
  }
private:
  template <class R>
  static decltype(auto) begin (R&& range) {
    using std::begin;
    return begin(std::forward<R>(range));
  }

  template <class R>
  static decltype(auto) end (R&& range) {
    using std::end;
    return end(std::forward<R>(range));
  }
private:
  iterator start;
  sentinel stop;
};

template <class I, class S>
void swap (iterable<I, S>& lhs, iterable<I, S>& rhs) noexcept { lhs.swap(rhs); }

} /* namespace apex */

#endif /* APEX_CORE_ITERABLE_HPP */
