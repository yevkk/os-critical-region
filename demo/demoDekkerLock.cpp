#include <iostream>
#include <thread>

#include "DekkerLock.hpp"
#include "utils/DemoTools.hpp"

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
                  << "Additions in each of 2 threads made: " << lab::demo::util::THREAD_OPERATION_REPEATS
                  << "\n     Resulting counter: " << counter << "\n";
    }
}

void inc(std::int32_t& arg)
{
    ++arg;
}

void demonstrate_data_race()
{
    std::cout << "\n***Demonstrating results of additions"
                 " and subtractions with no mutual exclusion primitives (data race demo)***\n";

    subtraction_addition_counter_demonstration(lab::demo::util::LoopDecorator{inc});
}

void avoid_data_race_with_dekker_lock()
{
    std::cout << "\n***Demonstrating results of additions and subtractions using DekkerLock.lock()***\n";
    lab::DekkerLock dekker_lock;

    subtraction_addition_counter_demonstration(
        lab::demo::util::LoopDecorator{
                    std::bind(lab::demo::util::ThreadSafeDecorator{inc}, std::ref(dekker_lock), std::placeholders::_1)
            }
    );
}

void avoid_data_race_with_dekker_lock_try()
{
    std::cout << "\n***Demonstrating results of additions and subtractions using DekkerLock.try_lock()***\n";
    lab::DekkerLock dekker_lock;

    subtraction_addition_counter_demonstration(
        lab::demo::util::LoopDecorator{
            std::bind(lab::demo::util::ThreadSafeTryDecorator{inc}, std::ref(dekker_lock), std::placeholders::_1)
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



