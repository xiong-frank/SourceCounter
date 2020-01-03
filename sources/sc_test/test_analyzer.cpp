#include <iostream>
#include "test_utils.h"

_SC_Test(_test_add)
{
    int a = 0;
    int b = 1;

    if (2 == (a + b))
        std::cout << "_test_add: true" << std::endl;
    else
        std::cout << "_test_add: false" << std::endl;
}

_SC_Test(_test_sub)
{
    int a = 0;
    int b = 1;

    if (1 == (b - a))
        std::cout << "_test_sub: true" << std::endl;
    else
        std::cout << "_test_sub: false" << std::endl;
}

