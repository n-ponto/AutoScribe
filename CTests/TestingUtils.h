#ifndef TESTING_UTILS_H_
#define TESTING_UTILS_H_

#include <string>

struct test
{
    void (*f)();
    std::string s;
};

void runTests(struct test*);

void assert(char check, std::string message);

#endif //TESTING_UTILS_H_