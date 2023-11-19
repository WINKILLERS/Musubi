#ifndef COMMON_HPP
#define COMMON_HPP
#include <type_traits>

namespace Network {
class IDeleteSelf {
public:
  // The same as "deleteLater"
  virtual void deleteSelf() = 0;
};

template <class _Ty> struct base_delete { // base deleter for unique_ptr
  constexpr base_delete() noexcept = default;

  template <class _Ty2,
            std::enable_if_t<std::is_convertible_v<_Ty2 *, _Ty *>, int> = 0>
  _CONSTEXPR23 base_delete(const base_delete<_Ty2> &) noexcept {}

  _CONSTEXPR23 void operator()(_Ty *_Ptr) const noexcept
  /* strengthened */ { // delete a pointer
    static_assert(0 < sizeof(_Ty), "can't delete an incomplete type");
    _Ptr->deleteSelf();
  }
};

using base_deleter = base_delete<IDeleteSelf>;
} // namespace Network
#endif