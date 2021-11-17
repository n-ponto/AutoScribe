#ifndef TESTING_UTILS_H_
#define TESTING_UTILS_H_

#include <string>

struct test
{
    void (*f)();
    std::string s;
};

void runTests(std::string title, struct test *tests);

void assert(char check);
void assert(char check, std::string message);

#endif //TESTING_UTILS_H_