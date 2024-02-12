#define TUNIT_IMPLEMENTATION
#include "tunit.h"

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
    t_initTestSuite("sums");
    t_addTestToSuite("basic-sum", test4);
    t_addTestToSuite("basic-sum-reversed", test5);

    t_initTestSuite("dumb");
    t_addTestToSuite("great", greatTest);
    t_addTestToSuite("test3", test3);

    t_initTestSuite("empty");

    return t_runSuites(argc, argv);
}
