#ifndef APEX_ITERABLE_HPP
#define APEX_ITERABLE_HPP

#include <iterator>

// Sort of like a range type, but it is not type erased. This is a simpler form
// of a C++20 ranges API, but it's more so you can easily construct ranges for
// use from old iterator pairs with little effort.

template <class I, class S=I>
struct iterable final {
  using iterator = I;
  using sentinel = S;

  template <class T, class U>
  iterable (T&& start, U&& stop) :
    start { std::forward<T>(start) },
    stop { std::forward<T>(stop) }
  { }

  void swap (iterable& that) noexcept {
    using std::swap;
    swap(this->start, that.start);
    swap(this->stop, that.stop);
  }

  explicit operator bool () const noexcept { return this->start != this->stop; }

  decltype(auto) operator * () const noexcept { return *this->start; }
  auto operator -> () const noexcept { return std::addressof(**this); }

  iterable operator ++ (int) { return iterable(this->start++, this->stop); }
  iterable& operator ++ () noexcept { return ++this->this; return *this; }

  iterator begin () const noexcept { return this->start; }
  sentinel end () const noexcept { return this->stop; }

  auto reverse () const noexcept {
    return iterable {
      std::make_reverse_iterator(this->stop),
      std::make_reverse_iterator(this->start)
    };
  }
private:
  template <class R>
  static begin (R&& range) {
    using std::begin;
    return begin(std::forward<R>(range));
  }

  template <class R>
  static end (R&& range) {
    using std::end;
    return end(std::forward<R>(range));
  }

  iterator start;
  sentinel stop;
};

template <class I, class S>
void swap (iterable<I, S>& lhs, iterable<I, S>& rhs) noexcept { lhs.swap(rhs); }

#endif /* APEX_ITERABLE_HPP */
