#pragma once

#include <atomic>

namespace lab {

/**
 * @brief Enhanced implemantion of Lamport's bakery algorithm
 *  for mutual exclusion using atomic operations.
 */
class ImprovedBakeryLock
{
public:
    /**
     * @brief Block until lock is acquired.
     */
    void lock() noexcept
    {
        /// Acquire unique ticket
        const auto ticket = _counter.fetch_add(1, std::memory_order_relaxed) + 1;

        /// Fence to ensure ticket was acquired before
        std::atomic_thread_fence(std::memory_order_acquire);

        /// Wait until our turn
        while (_current.load(std::memory_order_relaxed) != ticket);
    }

    /**
     * @brief Release lock.
     */
    void unlock() noexcept
    {
        /// Allow next thread to acquire a lock
        _current.fetch_add(1, std::memory_order_release);
    }

    /**
     * @brief Try to acquire lock and immediately return.
     * @return @c True if lock was successfully acquired, @c false otherwise.
     */
    [[nodiscard]]
    bool try_lock() noexcept
    {
        /// Assume we are going to acquire a lock
        auto ticket = _counter.load(std::memory_order_relaxed) + 1;

        /// Check whether ticket is available
        if (_current.load(std::memory_order_acquire) != ticket) {
            return false;
        }

        /// Check whether counter was modified in between
        auto expected = ticket - 1;
        if (!_counter.compare_exchange_strong(expected, ticket, std::memory_order_release)) {
            return false;
        }

        return true;
    }

private:
    std::atomic_uint64_t _counter{0};
    std::atomic_uint64_t _current{1};
};

} // namespace lab
