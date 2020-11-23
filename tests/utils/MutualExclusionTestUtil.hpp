#pragma once

#include <thread>
#include <array>
#include <cstdint>
#include <limits>
#include <chrono>
#include <mutex>

#include "../../src/FixnumLockable.hpp"

namespace lab::test::util {

/**
 * @brief Test function, that tests whether given FixnumLockable primitive is thread safe
 * @tparam L given primitive's type that follows FixnumLockable concept
 * @tparam N threads number, by default its value is 2
 * @param lock mutual exclusion primitive to test
 * @returns true - if primitive is thread safe, else false
 */
template <FixnumLockable L, std::size_t N = 2>
auto check_mutual_exclusion_condition(L&& lock) -> bool
{
    const std::uint32_t bound = 10e3;
    std::uint32_t counter = 0u;
    std::array<std::thread, N> threads;
    std::atomic_bool lock_is_acquired = false;
    std::atomic_bool got_collision = false;
    for (std::size_t i = 0; i < N; ++i) {
        threads[i] = std::thread(
            [&]() {
                while (counter < bound) {
                    std::scoped_lock lk(lock);
                    if (lock_is_acquired.exchange(true)) {
                        got_collision.store(true);
                    }
                    std::this_thread::sleep_for(std::chrono::microseconds((i % 2) * 100));
                    ++counter;
                    lock_is_acquired.store(false);
                }
            }
        );
    }

    for (std::size_t i = 0; i < N; ++i) {
        threads[i].join();
    }

    return !got_collision.load();
}

} // namespace lab::test::util

