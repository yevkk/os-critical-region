#include <iostream>
#include <iomanip>
#include <thread>

#include "DekkerLock.hpp"

const std::size_t THREAD_OPERATION_REPEATS = 100000;
const std::size_t RUNS_NUMBER = 10;

template <class Subtraction, class Addition>
void subtraction_addition_counter_demonstration(Subtraction subtraction, Addition addition)
{
    for (int i = 0; i < RUNS_NUMBER; ++i) {
        std::int32_t counter = 0;
        std::thread addition_thread(addition, std::ref(counter));
        std::thread subtraction_thread(subtraction, std::ref(counter));

        addition_thread.join();
        subtraction_thread.join();

        std::cout << std::setfill('.') << std::setw(5) << std::left << i + 1
                  << "Additions and Subtractions made: " << THREAD_OPERATION_REPEATS
                  << "\n     Resulting counter: " << counter << "\n";
    }
}

void demonstrate_data_race()
{
    std::cout << "\n***Demonstrating results of additions"
                 " and subtractions with no mutual exclusion primitives (data race demo)***\n";

    auto subtraction = [](std::int32_t& counter) {
        for (std::size_t i = 0; i < THREAD_OPERATION_REPEATS; ++i) {
            --counter;
        }
    };
    auto addition = [](std::int32_t& counter) {
        for (std::size_t i = 0; i < THREAD_OPERATION_REPEATS; ++i) {
            ++counter;
        }
    };
    subtraction_addition_counter_demonstration(subtraction, addition);
}

void avoid_data_race_with_dekker_lock()
{
    std::cout << "\n***Demonstrating results of additions and subtractions using DekkerLock.lock()***\n";
    lab::DekkerLock dekker_lock;
    auto subtraction = [&dekker_lock](std::int32_t & counter) {
        for (std::size_t i = 0; i < THREAD_OPERATION_REPEATS; ++i) {
            dekker_lock.lock();
            --counter;
            dekker_lock.unlock();
        }
    };
    auto addition = [&dekker_lock](std::int32_t& counter) {
        for (std::size_t i = 0; i < THREAD_OPERATION_REPEATS; ++i) {
            dekker_lock.lock();
            ++counter;
            dekker_lock.unlock();
        }
    };
    subtraction_addition_counter_demonstration(subtraction, addition);
}

void avoid_data_race_with_dekker_lock_try()
{
    std::cout << "\n***Demonstrating results of additions and subtractions using DekkerLock.try_lock()***\n";
    lab::DekkerLock dekker_lock;
    auto subtraction = [&dekker_lock](std::int32_t& counter) {
        for (std::size_t i = 0; i < THREAD_OPERATION_REPEATS; ++i) {
            while(!dekker_lock.try_lock());
            --counter;
            dekker_lock.unlock();
        }
    };
    auto addition = [&dekker_lock](std::int32_t& counter) {
        for (std::size_t i = 0; i < THREAD_OPERATION_REPEATS; ++i) {
            while(!dekker_lock.try_lock());
            ++counter;
            dekker_lock.unlock();
        }
    };
    subtraction_addition_counter_demonstration(subtraction, addition);
}

auto main(int argc, char** argv) -> int
{
    demonstrate_data_race();
    avoid_data_race_with_dekker_lock();
    avoid_data_race_with_dekker_lock_try();
    return 0;
}



