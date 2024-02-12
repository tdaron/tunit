#define TUNIT_IMPLEMENTATION
#include "tunit.h"
#include <unistd.h>

void greatTest()
{
    t_assert_int(5, ==, 4);
}

void test3()
{
    t_assert_int(2, ==, 2);
}

int sum(int a, int b)
{
    return a + b;
}

void test4()
{
    t_assert_int(4, ==, sum(3, 1));
}

void test5()
{
    t_assert_int(4, ==, sum(1, 3));
}

int main(int argc, char** argv)
{
    testsuite_t* sums = t_createTestSuite("sums");
    t_addTestToSuite(sums, "basic-sum", test4);
    t_addTestToSuite(sums, "basic-sum-reversed", test5);

    testsuite_t* dumb = t_createTestSuite("dumb");
    t_addTestToSuite(dumb, "great", greatTest);
    t_addTestToSuite(dumb, "test3", test3);

    t_createTestSuite("empty");

    return t_runSuites(argc, argv);
}
