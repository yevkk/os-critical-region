#pragma once

#include "FixnumLockable.hpp"
#include "FixnumLockException.hpp"

#include <mutex>
#include <array>
#include <thread>
#include <algorithm>

namespace lab {

    /**
     *  @brief Base class for mutual exclusion primitives with limited number of threads
     *  @tparam N threads number
     */
    template <std::size_t N>
    class FixnumLockableBase {

    protected:
        FixnumLockableBase() = default;

    public:
        /**
         *  @return Currect execution thread ID for locking, std::nullopt if thread is not registered
         */
        [[nodiscard]]
        auto get_id() const noexcept -> std::optional<ThreadId>
        {
            return _id;
        }

        /**
         *  @brief Registers currect execution thread for locking, assign ID to thread, does nothing if thread already registered
         *  @throws MaxThreadReachedException
         */
        auto register_thread() -> bool
        {
            if (_id) {
                return false;
            }
            std::scoped_lock lock{_mut};
            if (auto it = std::find(_threads.begin(), _threads.end(), std::thread::id{}); it != _threads.end()) {
                *it = std::this_thread::get_id();
                _id = std::distance(_threads.begin(), it);
                return true;
            }
            else {
                throw MaxThreadReachedException{};
            }
        }

        /**
         *  @brief Unregisters currect execution thread for locking, does nothing if thread is not registered
         */
        auto unregister_thread() noexcept -> bool
        {
            if (!_id) {
                return false;
            }
            std::scoped_lock lock{_mut};
            const auto it = std::find(_threads.begin(), _threads.end(), std::this_thread::get_id());
            *it = std::thread::id{};
            _id.reset();
            return true;
        }

    private:
        std::array<std::thread::id, N> _threads;
        std::mutex _mut;
        static inline thread_local std::optional<ThreadId> _id{};
    };
}