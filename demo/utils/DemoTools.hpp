#pragma once

#include <functional>
#include <iomanip>

namespace lab::demo::util {

    constexpr std::size_t THREAD_OPERATION_REPEATS = 500000;

    template <class Function>
    class BaseDecorator {
    public:
        explicit BaseDecorator(Function&& function) : _function(std::move(function)) {};
    protected:
        Function _function;
    };

    template <class Function>
    class LoopDecorator : public BaseDecorator<Function> {
    public:
        explicit LoopDecorator(Function function) : BaseDecorator<Function>(std::move(function)) {};

        using BaseDecorator<Function>::_function;

        void operator()(std::int32_t& arg)
        {
            for (std::size_t i = 0; i < THREAD_OPERATION_REPEATS; ++i) {
                _function(std::ref(arg));
            }
        }
    };

    template <class Function>
    class ThreadSafeDecorator : public BaseDecorator<Function> {
    public:
        explicit ThreadSafeDecorator(Function function) : BaseDecorator<Function>(std::move(function)) {};

        using BaseDecorator<Function>::_function;

        template <class Lock>
        void operator()(Lock& lock, std::int32_t& arg)
        {
            std::scoped_lock lk(lock);
            _function(arg);
        }

    };

    template <class Function>
    class ThreadSafeTryDecorator : BaseDecorator<Function> {
    public:
        explicit ThreadSafeTryDecorator(Function function) : BaseDecorator<Function>(std::move(function)) {};

        using BaseDecorator<Function>::_function;

        template <class Lock>
        void operator()(Lock& lock, std::int32_t& arg)
        {
            while (!lock.try_lock());
            _function(arg);
            lock.unlock();
        }
    };
}