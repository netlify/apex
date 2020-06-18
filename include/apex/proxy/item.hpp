#ifndef APEX_PROXY_ITEM_HPP
#define APEX_PROXY_ITEM_HPP

// Used as a proxy object when returning an 'assignable' value from operator []
// Effectively, it allows us to have a `operator []=`, which is not currently
// possible in C++.
// The reason this type is named 'item' is because operator [] is equal to
// Python's __getitem__, while apex::proxy::item::operator = is equal to
// Python's __setitem__

namespace apex::proxy {

template <class T>
struct item final {

};

} /* namespace apex::proxy */

#endif /* APEX_PROXY_ITEM_HPP */
