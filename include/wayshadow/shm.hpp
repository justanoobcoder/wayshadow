#ifndef WAYSHADOW_SHM_HPP
#define WAYSHADOW_SHM_HPP

#include <cstddef>

namespace wayshadow {

class ShmPool {
 public:
  ShmPool() = default;
  ~ShmPool() = default;

  // Disallow copy, allow move
  ShmPool(const ShmPool&) = delete;
  ShmPool& operator=(const ShmPool&) = delete;
  ShmPool(ShmPool&&) noexcept = default;
  ShmPool& operator=(ShmPool&&) noexcept = default;

  [[nodiscard]] static int AllocateShmFile(size_t size) noexcept;
};

}  // namespace wayshadow

#endif
