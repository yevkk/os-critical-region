#pragma once

#include "FixnumLockableBase.hpp"

#include <array>
#include <atomic>

namespace lab {
    
    using Ticket = int;

    /**
     *  @brief Mutual exclusion primitive which uses Lampert's Bakery Algorithm
     *  @tparam N threads number
     */
    template <std::size_t N>
    class BakeryLock : public FixnumLockableBase<N> {
        
    public:
        BakeryLock() : FixnumLockableBase<N>() {}

        using FixnumLockableBase<N>::get_id;

        using FixnumLockableBase<N>::register_thread;

        /**
         *  @throws MaxThreadReachedException
         */
        auto lock() -> void
        {
            ThreadId id = _check_if_registered();
           
            _get_ticket(id);
            for (std::size_t j = 0; j < N; ++j) {
                if(j<id)
                    while (_entering[j]) {}
                while ((_tickets[j] != 0) &&                                // ticket = 0 means thread j isn't around critical section
                        ((_tickets[j] < _tickets[id])                       // thread j came first so served first
                        || ((_tickets[j] == _tickets[id]) && (j < id))))    // thread j came at the same time but has higher priority
                {}
            }
        }

        /**
         *  @throws MaxThreadReachedException 
         */
        auto unlock() -> void 
        {           
            ThreadId id = _check_if_registered();
            _tickets[id] = 0;
        }

        /**
         *  @return true if exclusive access was acquired, false if another Thread already has exclusive access 
         *  @throws MaxThreadReachedException
         */
        auto try_lock() -> bool
        {
            ThreadId id = _check_if_registered();
            
            _get_ticket(id);
            for (std::size_t j = 0; j < N; ++j) {
                if (j < id)
                    while (_entering[j]) {}
                if (_tickets[j] != 0)                                      // ticket = 0 means thread j isn't around critical section
                    if ((_tickets[j] < _tickets[id])                       // thread j came first so served first
                        || ((_tickets[j] == _tickets[id]) && (j < id)))    // thread j came at the same time but has higher priority
                    {
                        _tickets[id] = 0;
                        return false;
                    }
            }
            return true;
        }

    private:
        /**
         *  @brief Utility function for giving thread a ticket
         */
        void _get_ticket(ThreadId id) {
            _entering[id] = true;
            Ticket max_ticket = 0;
            for (int t = 0; t < N; ++t)
                if (_tickets[t] > max_ticket)
                    max_ticket = _tickets[t];
            _tickets[id] = ++max_ticket;
            _entering[id] = false;
        }

        /**
         *  @brief Utility function for checking if thread is registered, tries to register thread if it's not registered 
         *  @return Thread id if thread is registered
         *  @throws MaxThreadReachedException
         */
        ThreadId _check_if_registered() {
            std::optional<ThreadId> id = get_id();
            if (id == std::nullopt) {
                register_thread();
                id = get_id();
            }
            return id.value();
        }

        
    private:
        std::array<std::atomic<bool>,N> _entering;
        std::array<std::atomic<Ticket>, N> _tickets;
    };
}