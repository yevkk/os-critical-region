#include <iostream>
#include <iomanip>
#include <thread>

#include "DekkerLock.hpp"

constexpr std::size_t THREAD_OPERATION_REPEATS = 100000;
constexpr std::size_t RUNS_NUMBER = 10;

template <class Function>
void subtraction_addition_counter_demonstration(Function function)
{
    for (int i = 0; i < RUNS_NUMBER; ++i) {
        std::int32_t counter = 0;
        std::thread parallel_thread(function, std::ref(counter));
        function(counter);

        parallel_thread.join();

        std::cout << std::setfill('.') << std::setw(5) << std::left << i + 1
                  << "Additions in each of 2 threads made: " << THREAD_OPERATION_REPEATS
                  << "\n     Resulting counter: " << counter << "\n";
    }
}

void inc(std::int32_t& arg)
{
    ++arg;
}

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

void demonstrate_data_race()
{
    std::cout << "\n***Demonstrating results of additions"
                 " and subtractions with no mutual exclusion primitives (data race demo)***\n";

    subtraction_addition_counter_demonstration(LoopDecorator{inc});
}

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

void avoid_data_race_with_dekker_lock()
{
    std::cout << "\n***Demonstrating results of additions and subtractions using DekkerLock.lock()***\n";
    lab::DekkerLock dekker_lock;

    subtraction_addition_counter_demonstration(
            LoopDecorator{
                    std::bind(ThreadSafeDecorator{inc}, std::ref(dekker_lock), std::placeholders::_1)
            }
    );
}

template <class Function>
class ThreadSafeTryDecorator : BaseDecorator<Function> {
public:
    explicit ThreadSafeTryDecorator(Function function) : BaseDecorator<Function>(std::move(function)) {};

    using BaseDecorator<Function>::_function;

    template <class Lock>
    void operator()(Lock& lock, std::int32_t& arg)
    {
        while(!lock.try_lock());
        _function(arg);
        lock.unlock();
    }
};

void avoid_data_race_with_dekker_lock_try()
{
    std::cout << "\n***Demonstrating results of additions and subtractions using DekkerLock.try_lock()***\n";
    lab::DekkerLock dekker_lock;

    subtraction_addition_counter_demonstration(
        LoopDecorator{
            std::bind(ThreadSafeTryDecorator{inc}, std::ref(dekker_lock), std::placeholders::_1)
        }
    );
}

auto main(int argc, char** argv) -> int
{
    demonstrate_data_race();
    avoid_data_race_with_dekker_lock();
    avoid_data_race_with_dekker_lock_try();
    return 0;
}



