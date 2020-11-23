#include "DekkerLock.hpp"

namespace lab{

DekkerLock::DekkerLock()
    : _thread_wants_to_enter{false, false}
    , _turn(0u)
{}

void DekkerLock::lock()
{
    const ThreadId this_thread_id = _get_registered_id();
    const ThreadId another_thread_id = _get_another_thread_id();
    _thread_wants_to_enter[this_thread_id].store(true, std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acq_rel);
    while (_thread_wants_to_enter[another_thread_id].load(std::memory_order_relaxed)) {
        if (_turn.load(std::memory_order_relaxed) != this_thread_id) {
            _thread_wants_to_enter[this_thread_id].store(false, std::memory_order_relaxed);
            std::atomic_thread_fence(std::memory_order_acq_rel);
            while (_turn.load(std::memory_order_relaxed) != this_thread_id);
            std::atomic_thread_fence(std::memory_order_acquire);
            _thread_wants_to_enter[this_thread_id].store(true, std::memory_order_relaxed);
        }
    }
}

void DekkerLock::unlock()
{
    _turn.store(_get_another_thread_id(), std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_release);
    _thread_wants_to_enter[get_id().value()].store(false, std::memory_order_relaxed);
    unregister_thread();
}

auto DekkerLock::try_lock() -> bool
{
    const ThreadId another_thread_id = _get_another_thread_id();
    _thread_wants_to_enter[get_id().value()].store(true, std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acq_rel);
    while (_thread_wants_to_enter[another_thread_id].load(std::memory_order_relaxed)) {
        if (_turn.load(std::memory_order_relaxed) == another_thread_id) {
            _thread_wants_to_enter[get_id().value()].store(false, std::memory_order_relaxed);
            return false;
        }
    }
    return true;
}

auto DekkerLock::_get_registered_id() -> ThreadId
{
    auto id = get_id();
    if (!id.has_value()) {
        register_thread();
        id = get_id();
    }
    return id.value();
}

auto DekkerLock::_get_another_thread_id() -> ThreadId {
    return 1u - _get_registered_id();
}

} // namespace lab
