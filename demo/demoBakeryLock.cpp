#include "../src/BakeryLock.hpp"
#include "../src/FixnumLockException.hpp"
#include "utils/DemoTools.hpp"
#include <thread>
#include <chrono>
#include <iostream>

constexpr int THREADS = 5;
constexpr std::size_t N = THREADS - 1;

std::int32_t iterations[N];

bool volatile start = false;
bool volatile stop = false;

std::int32_t count = 0;
lab::BakeryLock<THREADS> bakery_lock;

void inc(std::size_t id, std::int32_t& arg) {
    ++arg;
    ++iterations[id];
}

void dec(std::size_t id, std::int32_t& arg) {
    --arg;
    ++iterations[id];
}

template<class Function>
void execute(Function function) {
    bakery_lock.register_thread();
    std::size_t id = *bakery_lock.get_id();

    //while (!start);
    
    lab::demo::util::LoopDecorator{
        std::bind(lab::demo::util::ThreadSafeDecorator{
            std::bind(function, id, std::placeholders::_1)
        },
        std::ref(bakery_lock),
        std::placeholders::_1
    ) }(count);
}

void monitor() {
    while (!stop) {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        {
            std::lock_guard lk(bakery_lock);
            std::cout << std::endl << count << std::endl;
            for (int i = 0; i < N; ++i)
                std::cout << "Thread " << i << " made " << iterations[i] << " iterations\n";
            std::cout << std::endl;
            std::fill(iterations, iterations + N, 0);
        }       
    }
}

void demonstrate() {
    std::thread thr[N];
    for (int i = 0; i < N; ++i) {
        if (i % 2 == 0)
            thr[i] = std::thread{ execute<decltype(inc)>, inc };
        else
            thr[i] = std::thread{ execute<decltype(dec)>, dec };
    }
    std::thread m = std::thread(monitor);
    start = true;

    for (int i = 0; i < N; ++i)
        thr[i].join();
    stop = true;
    m.join();
}


auto main(int argc, char** argv) -> int
{
    demonstrate();
    return 0;
}