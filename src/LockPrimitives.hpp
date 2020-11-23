#pragma once

namespace lab
{
    class DummyLock {
    public:
        void lock() noexcept { }
        void unlock() noexcept { }
    };

} //namespace lab
