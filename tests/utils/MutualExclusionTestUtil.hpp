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
    const std::uint32_t bound = 10e6;
    std::uint32_t counter = 0u;
    std::array<std::thread, N> threads;
    for (std::size_t i = 0; i < N; ++i) {
        threads[i] = std::thread(
            [i, &lock, bound, &counter]() {
                while (true) {
                    std::scoped_lock lk(lock);
                    if (counter >= bound) {
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::microseconds(i * 500));
                    counter += 10000;
                }
            }
        );
    }

    for (std::size_t i = 0; i < N; ++i) {
        threads[i].join();
    }

    return counter == bound;
}

} // namespace lab::test::util

