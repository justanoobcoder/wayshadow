#include "keypop/shm.hpp"

#include <cstdio>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace keypop {

    namespace {
        [[nodiscard]] int create_shm_file() noexcept {
            static unsigned int counter = 0;
            char name[32];
            std::snprintf(name, sizeof(name), "/wl_shm-%u-%u", static_cast<unsigned int>(getpid()), counter++);

            // O_EXCL ensures we create a fresh, uncollided anonymous shared memory file
            const int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
            if (fd >= 0) {
                // Unlink immediately so file descriptor cleans up on close
                shm_unlink(name);
            }
            return fd;
        }
    } // namespace

    int ShmPool::allocate_shm_file(size_t size) noexcept {
        const int fd = create_shm_file();
        if (fd < 0) {
            return -1;
        }
        if (ftruncate(fd, static_cast<off_t>(size)) < 0) {
            close(fd);
            return -1;
        }
        return fd;
    }

} // namespace keypop
