#include "../src/BakeryLock.hpp"
#include "../src/FixnumLockException.hpp"
#include <thread>
#include <iostream>
#include <string>

const int THREADS = 7;
const size_t N = 6;
int count = 0;
lab::BakeryLock<N> bl;

void change(const bool add) {
    try {
        for (int i = 0; i < 5; ++i) {
            bl.lock();
            if (add) {
                ++count;
                std::cout << "Thread  " << bl.get_id().value() << " added 1 and count is " << count << '\n';
            }
            else {
                --count;
                std::cout << "Thread " << bl.get_id().value() << " subtracted 1 and count is " << count << '\n';
            }
            bl.unlock();
        }
    }
    catch (lab::MaxThreadReachedException e) {
        std::cout << e.what() << '\n';
    }
}

auto main(int argc, char** argv) -> int
{
    std::thread thr[THREADS];
    for (int i = 0; i < THREADS; ++i) {
        if (i % 2 == 0)
            thr[i] = std::thread(change, true);
        else
            thr[i] = std::thread(change, false);
    }

    for (int i = 0; i < THREADS; ++i)
        thr[i].join();
    return 0;
}