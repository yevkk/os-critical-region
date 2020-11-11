#pragma once

#include <atomic>

#include "FixnumLockableBase.hpp"
#include "FixnumLockException.hpp"

namespace lab {

/**
 * @brief Mutual exclusion primitive which uses Dekker's locking algorithm for 2 threads
 */
class DekkerLock : public FixnumLockableBase<2> {
public: // methods
    DekkerLock();

    /**
     * @brief Give current execution thread exclusive access, if access is given to another thread, busy waiting is applied
     * @throws MaxThreadReachedException
     */
    void lock();

    /**
     * @brief Try to give current execution thread exclusive access
     * @return true - if access is successfully given to current execution thread, false - if access is already given to another thread
     * @throws MaxThreadReachedException
     */
    auto try_lock() -> bool;

    /**
     * @brief Release an exclusive access right from current execution thread
     * @throws MaxThreadReachedException
     */
    void unlock();

private: // methods

    /**
     * @brief Utility method to get current execution thread id. If thread is not registered yet, it gets registered
     * @return registration id of the current execution thread
     * @throws MaxThreadReachedException
     */
    auto _get_registered_id() -> ThreadId;

    /**
     * @brief Utility method to get other execution thread registration id
     * @return registration id of another execution thread
     */
     auto _get_another_thread_id() -> ThreadId;

private: // fields
    std::array<std::atomic_bool, 2> _thread_wants_to_enter;
    std::atomic_size_t _turn;
};

} // namespace lab



