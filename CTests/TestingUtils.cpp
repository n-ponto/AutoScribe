#include <string>
#include <iostream>

#include "TestingUtils.h"

void runTests(struct test *tests)
{
    int fail = 0;
    for (struct test *t = tests; t->f != 0; t++) {
        std::cout << t->s << ": ";
        try
        {
            t->f(); // run the test
            std::cout << "OK" << std::endl;
        }
        catch(const std::exception& e)
        {
            fail = 1;
            std::cout << "FAIL" << std::endl;
        }
    }

    if(fail)
    {
        std::cout << "SOME TESTS FAILED" << std::endl;
        exit(1);
    }

    std::cout << "ALL TESTS PASSED" << std::endl;
}

void assert(char check, std::string message)
{
    if (!check)
    {
        std::cout << "\n[ASSERT FAILURE]: " << message << std::endl;
        throw std::exception();
    }
}