#pragma once

#include <atomic>

namespace lab
{
    class DummyLock {
    public:
        void lock() noexcept { }
        void unlock() noexcept { }
    };

    class SpinLock {
    public:
        SpinLock() : _lock{false} {}

        void lock()
        {
            while(true) {
                if (!_lock.exchange(true, std::memory_order_acquire))
                {
                    break;
                }
                while (_lock.load(std::memory_order_relaxed));
            }
        }

        void unlock()
        {
            _lock.store(false, std::memory_order_release);
        }

    private:
        std::atomic<bool> _lock;
    };

} //namespace lab
