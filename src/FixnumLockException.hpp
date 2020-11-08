#pragma once

#include "FixnumLockable.hpp"

#include <string>
#include <stdexcept>

namespace lab {

    class FixnumLockException : public std::runtime_error {

    public:
        FixnumLockException (const std::string& what)
            : std::runtime_error {what.data()}
        {}
    };

    class MaxThreadReachedException : public FixnumLockException {

    public:
        MaxThreadReachedException()
            : FixnumLockException {"Max amount of threads registered"}
        {}
    };

}