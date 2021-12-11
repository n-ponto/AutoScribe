#ifndef TESTING_UTILS_H_
#define TESTING_UTILS_H_

#include <string>

struct test
{
    void (*f)();   // Function pointer to the test to run
    std::string s; // Name of the test (should match function name)
};

void runTests(std::string title, struct test *tests);

void assert(char check);
void assert(char check, std::string message);

#endif //TESTING_UTILS_H_