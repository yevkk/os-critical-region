#pragma once

#include "FixnumLockable.hpp"
#include "FixnumLockException.hpp"

#include <mutex>
#include <array>
#include <thread>
#include <algorithm>
#include <shared_mutex>
#include <unordered_map>

namespace lab {

    /**
     *  @brief Base class for mutual exclusion primitives with limited number of threads
     *  @tparam N threads number
     */
    template <std::size_t N>
    class FixnumLockableBase {

    protected:
        FixnumLockableBase()
        {
            _ids.reserve(N);
        }

    public:
        /**
         *  @return Currect execution thread ID for locking, std::nullopt if thread is not registered
         */
        auto get_id() const noexcept -> std::optional<ThreadId>
        {
            std::shared_lock lock{_mut};
            if (auto it = _ids.find(std::this_thread::get_id()); it != _ids.end()) {
                return it->second;
            }
            else {
                return std::nullopt;
            }
        }

        /**
         *  @brief Registers currect execution thread for locking, assign ID to thread, does nothing if thread already registered
         *  @throws MaxThreadReachedException
         */
        auto register_thread() -> void
        {
            std::scoped_lock lock{_mut};
            if (_ids.find(std::this_thread::get_id()) == _ids.end()) {
                if (_ids.size() == N) {
                    throw MaxThreadReachedException{};
                }
                const auto it = std::find(_threads.begin(), _threads.end(), std::thread::id{});
                *it = std::this_thread::get_id();
                _ids.emplace(*it, std::distance(_threads.begin(), it));
            }
        }

        /**
         *  @brief Unregisters currect execution thread for locking, does nothing if thread is not registered
         */
        auto unregister_thread() noexcept -> void
        {
            std::scoped_lock lock{_mut};
            if (auto it = _ids.find(std::this_thread::get_id()); it != _ids.end()) {
                _threads[it->second] = std::thread::id{};
                _ids.erase(it);
            }
        }

    private:
        std::unordered_map<std::thread::id, ThreadId> _ids;
        std::array <std::thread::id, N> _threads;
        mutable std::shared_mutex _mut;
    };
}