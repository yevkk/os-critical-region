#include "../src/LockPrimitives.hpp"
#include "../src/DekkerLock.hpp"
#include "../src/BakeryLock.hpp"
#include "../src/ImprovedBakeryLock.hpp"

#include <iostream>
#include <atomic>
#include <mutex>
#include <future>
#include <chrono>
#include <vector>
#include <string>


namespace lab::utils
{
    template<typename T>
    class Incrementable {
    public:
        explicit Incrementable(T value) noexcept :
            _value{std::move(value)}
        {}

        void increment() noexcept
        {
            ++_value;
        }

        auto value() -> T&
        {
            return _value;
        }

    private:
        T _value;
    };

    template<typename T>
    class Incrementable<std::atomic<T>> {
    public:
        explicit Incrementable(T value) :
                _value{std::move(value)}
        { }

        void increment()
        {
            _value.fetch_add(1, std::memory_order_relaxed);
        }

        auto value() -> std::atomic<T>&
        {
            return _value;
        }

    private:
        std::atomic<T> _value;
    };

    template<typename P, typename V>
    class LockableValue {
    public:
        class Proxy {
        public:
            explicit Proxy (P& primitive, V& value) :
                    _lock{primitive},
                    _value{value}
            { }

            [[nodiscard]]
            auto value () -> V&
            {
                return _value;
            }

        private:
            std::lock_guard<P> _lock;
            V& _value;
        };

        template<typename... Args>
        explicit LockableValue(Args&&...args) :
                _value{std::forward<Args...>(args...)}
        { }

        [[nodiscard]]
        auto lock() -> Proxy
        {
            return Proxy{_primitive, _value};
        }

    private:
        V _value;
        P _primitive;
    };

    constexpr std::size_t N = 1e7;
    constexpr std::size_t THREADS_NO = 4;
    template<typename TUM, typename P, typename V> //TUM - Time Unit of Measurement
    int benchmark(
            LockableValue<P, V>&& lockable_value,
            std::size_t threads_number = THREADS_NO,
            std::size_t n = N)
    {
        using namespace std::chrono;
        auto start_ts = system_clock::now();

        const auto func = [&lockable_value](std::size_t n)
                {
                    for (std::size_t i = 0; i < n; i++) {
                        lockable_value.lock().value().increment();
                    }
                };

        std::vector<std::future<void>> futures;
        futures.reserve(threads_number);
        for (std::size_t i = 0; i < threads_number - 1; i++) {
            futures.emplace_back(std::async(std::launch::async, func, n));
        }

        for (auto &fut : futures) {
            fut.wait();
        }

        return duration_cast<TUM>(system_clock::now() - start_ts).count();
    }


} //lab::utils

int main()
{
    using namespace lab;
    using namespace lab::utils;

    using TUM = std::chrono::milliseconds;

    std::cout << "N = " << N << std::endl;

    constexpr int first_col_size = 9;
    constexpr int second_col_size = 10;

    auto benchmark_primitive  = []<typename P, typename V>(const std::string& label,
                                                           LockableValue<P, V>&& lockable_value,
                                                           std::size_t max_threads = THREADS_NO) {
        std::cout << '[' << label << ']' << std::endl;
        std::cout
                << "| Threads | Time(ms) |" << std::endl
                << '|' << std::string(first_col_size, '-') << '|'
                << std::string(second_col_size, '-') << '|' << std::endl;
        for (std::size_t threads_number = 2; threads_number <= max_threads; threads_number++) {
            int res = benchmark<TUM>(std::move(lockable_value), threads_number);
            std::cout
                    << '|' << std::string(first_col_size - 1 - std::to_string(threads_number).length(), ' ')
                    << threads_number << " |"
                    << std::string(second_col_size - 1 - std::to_string(res).length(), ' ')
                    << res << " |"
                    << std::endl;
        }
        std::cout
                << '|' << std::string(first_col_size, '=') << '|'
                << std::string(second_col_size, '=') << '|' << std::endl;
        std::cout << std::endl;
    };


    benchmark_primitive("std::atomic", LockableValue<DummyLock, Incrementable<std::atomic_int>>{0});
    benchmark_primitive("std::mutex", LockableValue<std::mutex, Incrementable<int>>{0});
    benchmark_primitive("lab::SpinLock", LockableValue<SpinLock, Incrementable<int>>{0});
    benchmark_primitive("lab::DekkerLock", LockableValue<DekkerLock, Incrementable<int>>{0}, 2);
    benchmark_primitive("lab::ImprovedBakeryLock", LockableValue<ImprovedBakeryLock, Incrementable<int>>{0});


    return 0;
}
