#pragma once

#include <concepts>
#include <optional>

namespace lab {

    using ThreadId = std::size_t;

#if __cplusplus > 201703L

    template <typename T>
    concept BasicLockable = requires (T x)
    {
        x.lock();
        x.unlock();
    };

    template <typename T>
    concept Lockable = BasicLockable<T> && requires (T x)
    {
        {x.try_lock()} -> std::same_as<bool>;
    };

    template <typename T>
    concept FixnumLockable = Lockable<T> && requires(T x)
    {
        {x.get_id()} -> std::same_as<std::optional<ThreadId>>;
        {x.register_thread()};
        {x.unregister_thread()};
    };
#endif

}