#include <assert.h>
#include <stdio.h>
#define TUNIT_IMPLEMENTATION
#include "tunit.h"

/* Basics */
void greatTest()
{
    int b = 4.0 == 5.0;
    printf("some debug inside the test\n");
    printf("another debug\n");
    // theses assert are MEANT to fail.
    t_assert_int(5, ==, 6);
    t_assert_int(5, ==, 8);
}

void test3()
{
    t_assert_int(2, ==, 2);
    t_assert_false(0);
    t_assert_true(1);
    t_assert_str_neq("one", "two");
    t_assert_str_eq("three", "three");
    t_assert_float(4.0f, ==, 4.0f);
    t_assert_double(4.0, ==, 4.0);
    printf("hello world (wont be shown because test3 succeed)\n");
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

void testIterative(void* data)
{
    int* data_i = (int*)data;
    t_assert_int(data_i[0], ==, sum(data_i[1], data_i[2]));
}

/* More advanced tests with "complex" data-structures*/
typedef struct Person {
    int age;
} person_t;

void test6(void* input)
{
    person_t* i = input;
    t_assert_int(i->age, ==, sum(6, 12));
}

void init_person(void* data)
{
    person_t* t = data;
    t->age = 18;
}

void free_static_data(void* data)
{
    free(data);
}

int main(int argc, char** argv)
{
    testsuite_t* sums = t_registerTestSuite("sums");
    t_addTestToSuite(sums, "basic-sum", test4);
    t_addTestToSuite(sums, "basic-sum-reversed", test5);

    testsuite_t* dumb = t_registerTestSuite("dumb");
    t_addTestToSuite(dumb, "great (should fail)", greatTest);
    t_addTestToSuite(dumb, "test3", test3);

    // One way to assign static data.
    testsuite_t* people = t_registerTestSuite("people");
    person_t theo = { .age = 69 };
    test_t* test6_t = t_addTestToSuite(people, "person age (should fail)", test6);
    test6_t->static_data = (void*)&theo;

    // One other way
    person_t vlad = { .age = 18 };
    test_t* test6_t_bis = t_addTestToSuite(people, "person age", test6);
    t_addStaticDataToTest(test6_t_bis, (void*)&vlad);

    // Assign start_up and clean_up
    person_t* to_init = malloc(sizeof(person_t));
    test_t* test6_t_bis_2 = t_addTestToSuite(people, "person age 2", test6);
    test6_t_bis_2->static_data = to_init;
    test6_t_bis_2->start_up = init_person;
    test6_t_bis_2->clean_up = free_static_data;

    // This can also be assigned using methods (EQUIVALENT)
    t_addStartUpToTest(test6_t_bis_2, init_person);
    t_addCleanUpToTest(test6_t_bis_2, free_static_data);

    testsuite_t* suite = t_registerTestSuite("advanced");

    // This test will be ran 3 times with different input samples
    test_t* t = t_addTestToSuite(suite, "iteratible test", testIterative);
    int** data = malloc(3 * sizeof(int[3]));
    data[0] = (int[]) { 5, 2, 3 };
    data[1] = (int[]) { 7, 9, -2 };
    data[2] = (int[]) { 8, 4, 4 };
    t->static_data = data;
    t->data_length = 3;
    t->clean_up = free_static_data;
    t_registerTestSuite("empty");

    return t_runSuites(argc, argv);
}
