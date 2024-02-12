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

typedef struct Person {
    int age;
} person_t;

void test6(void* input)
{
    person_t* i = input;
    t_assert_int(i->age, ==, sum(6, 12));
}

int main(int argc, char** argv)
{
    testsuite_t* sums = t_registerTestSuite("sums");
    t_addTestToSuite(sums, "basic-sum", test4);
    t_addTestToSuite(sums, "basic-sum-reversed", test5);

    testsuite_t* dumb = t_registerTestSuite("dumb");
    t_addTestToSuite(dumb, "great (should fail)", greatTest);
    t_addTestToSuite(dumb, "test3", test3);

    testsuite_t* people = t_registerTestSuite("people");
    person_t theo = { .age = 69 };
    test_t* test6_t = t_addTestToSuite(people, "person age (should fail)", test6);
    t_addStaticDataToTest(test6_t, (void*)&theo);

    person_t vlad = { .age = 18 };
    test_t* test6_t_bis = t_addTestToSuite(people, "person age", test6);
    t_addStaticDataToTest(test6_t_bis, (void*)&vlad);

    t_registerTestSuite("empty");

    return t_runSuites(argc, argv);
}
