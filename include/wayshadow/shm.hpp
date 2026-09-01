#pragma once

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

        [[nodiscard]] static int allocate_shm_file(size_t size) noexcept;
    };

} // namespace wayshadow
