#include <iostream>
#include <iomanip>

#include "MutualExclusionTestUtil.hpp"
#include "DekkerLock.hpp"
#include "BakeryLock.hpp"


auto main(int argc, char** argv) -> int
{
    std::cout << "Result of BakeryLock<4u> mutual exclusion check:" << std::boolalpha <<
        lab::test::util::check_mutual_exclusion_condition<lab::BakeryLock<4u>, 4u>(lab::BakeryLock<4u>{}) << '\n';
    std::cout << "Result of DekkerLock mutual exclusion check:" << std::boolalpha <<
        lab::test::util::check_mutual_exclusion_condition<lab::DekkerLock, 2u>(lab::DekkerLock{}) << '\n';
    return 0;
}