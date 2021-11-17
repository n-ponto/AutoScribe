#include <string>
#include <iostream>

#include "TestingUtils.h"

void runTests(std::string title, struct test *tests)
{
    std::cout << "\n\r*** " << title << " Tests ***\n\r" << std::endl;
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
        std::cout << "SOME TESTS FAILED\n\r" << std::endl;
        exit(1);
    }

    std::cout << "ALL TESTS PASSED\n\r" << std::endl;
}

void assertionFailure(std::string message)
{
    std::cout << "\n[ASSERT FAILURE]: " << message << std::endl;
    throw std::exception();
}

void assert(char check)
{
    assert(check, "no message, set gdb breakpoint at \"assertionFailure()\" function.");
}

void assert(char check, std::string message)
{
    if (!check)
    {
        assertionFailure(message);
    }
}