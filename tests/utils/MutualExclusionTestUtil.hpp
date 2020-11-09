#pragma once

#include <thread>
#include <array>
#include <cstdint>
#include <limits>
#include <chrono>

#include "../../src/FixnumLockable.hpp"

namespace lab {
namespace test {
namespace util {

template <FixnumLockable L, std::size_t N>
auto check_mutual_exclusion_condition(L lock) -> bool
{
    const std::uint32_t bound = 10e7;
    std::uint32_t counter = 0u;
    std::array<std::thread, N> threads;
    for (std::size_t i = 0; i < N; ++i) {
        threads[i] = std::thread(
            [i, &lock, bound, &counter]() {
                while (true) {
                    lock.lock();
                    if (counter >= bound) {
                        lock.unlock();
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::microseconds(i * 100));
                    counter += 10000;
                    lock.unlock();
                }
            }
        );
    }

    for (std::size_t i = 0; i < N; ++i) {
        threads[i].join();
    }

    return counter == bound;
}
} // namespace util
} // namespace test
} // namespace lab

