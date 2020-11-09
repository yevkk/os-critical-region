#include "DekkerLock.hpp"

namespace lab{

DekkerLock::DekkerLock()
<<<<<<< HEAD
    : _thread_wants_to_enter{false, false}
    , _turn(0u)
{}
=======
    : FixnumLockableBase<2U>()
{
    _thread_wants_to_enter[0].store(false);
    _thread_wants_to_enter[1].store(false);
    _turn.store(0u);
}
>>>>>>> fa11eb9... [Task-1] Implement mutual exclusion checker, which tests whether mutual exclusion is working in Lockable primitives

void DekkerLock::lock()
{
    const ThreadId this_thread_id = _get_registered_id();
    const ThreadId another_thread_id = _get_another_thread_id();
    _thread_wants_to_enter[this_thread_id].store(true);
    while (_thread_wants_to_enter[another_thread_id].load()) {
        if (_turn.load() != this_thread_id) {
            _thread_wants_to_enter[this_thread_id].store(false);
            while (_turn.load() != this_thread_id);
            _thread_wants_to_enter[this_thread_id].store(true);
        }
    }
}

void DekkerLock::unlock()
{
    _turn.store(_get_another_thread_id());
    _thread_wants_to_enter[get_id().value()].store(false);
}

auto DekkerLock::try_lock() -> bool
{
    const ThreadId another_thread_id = _get_another_thread_id();
    _thread_wants_to_enter[get_id().value()].store(true);
    while (_thread_wants_to_enter[another_thread_id].load()) {
        if (_turn.load() == another_thread_id) {
            _thread_wants_to_enter[get_id().value()].store(false);
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
