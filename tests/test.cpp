#include <iostream>
#include <iomanip>

#include "MutualExclusionTestUtil.hpp"
#include "DekkerLock.hpp"
#include "BakeryLock.hpp"

class EmptyLock : public lab::FixnumLockableBase<2>
{
public:

    EmptyLock() : FixnumLockableBase<2>()
    {}

    void lock()
    {}

    void unlock()
    {}

    auto try_lock() -> bool
    {
        return true;
    }
};

auto main(int argc, char** argv) -> int
{
    std::cout << "Result of EmptyLock mutual exclusion check: " << std::boolalpha <<
        lab::test::util::check_mutual_exclusion_condition(EmptyLock{}) << std::endl;
    std::cout << "Result of BakeryLock<6> mutual exclusion check: " << std::boolalpha <<
        lab::test::util::check_mutual_exclusion_condition<lab::BakeryLock<6>, 6>(lab::BakeryLock<6>{}) << std::endl;
    std::cout << "Result of DekkerLock mutual exclusion check: " << std::boolalpha <<
        lab::test::util::check_mutual_exclusion_condition(lab::DekkerLock{}) << std::endl;
    return 0;
}