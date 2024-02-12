#include <stdio.h>
#include <stdlib.h>
#ifndef TUNIT_H
#define TUNIT_H 42
void t_initTestSuite(char *name);
void t_addTestToSuite(char *name, void (*test_fn)(void *));
int t_runSuites(int argc, char **argv);
#define t_assert_int(a, op, b)                                                 \
  if (!((a)op(b))) {                                                           \
    tunit_error = 1;                                                           \
    printf("ERROR> %s:%d - assertion failed %d %s %d\n", __FILE__, __LINE__,   \
           a, #op, b);                                                         \
  }
#endif
#define TUNIT_IMPLEMENTATION
#ifdef TUNIT_IMPLEMENTATION
typedef struct Test test_t; // forward declaration
struct Test {
  test_t *next;
  char *name;
  void (*test_fn)(void *);
};

typedef struct TestSuite testsuite_t; // forward declaration
typedef struct TestSuite {
  test_t *first;
  char *name;
  size_t length;
  testsuite_t *next;
} testsuite_t;

typedef struct TestSuiteList {
  testsuite_t *first;
  size_t length;
} testsuitelist_t;

testsuitelist_t suite_list = {NULL, 0};
int succeeded;
int tunit_error;
int tunit_total_errors = 0;
void t_initTestSuite(char *name) {
  testsuite_t *new_suite = (testsuite_t *)malloc(sizeof(testsuite_t));
  new_suite->name = name;
  new_suite->first = NULL;
  new_suite->next = suite_list.first;
  suite_list.first = new_suite;
  suite_list.length++;
}

void pv_t_runSuite(testsuite_t *t) {
  while (t->first != NULL) {
    test_t *test = t->first;
    printf("\t--> %s\n", test->name);
    test->test_fn(NULL);
    if (tunit_error == 0) {
      succeeded++;
    } else {
      tunit_total_errors = 1;
    }
    tunit_error = 0;
    t->first = test->next;
    free(test);
  }
}

int t_runSuites(int argc, char **argv) {
  while (suite_list.first != NULL) {
    testsuite_t *suite = suite_list.first;
    if (suite->length == 0) {
      printf("Skipping %s because not tests in here\n", suite->name);
    } else {
      printf("Running %s\n", suite->name);
      pv_t_runSuite(suite);
      printf("Succeeded %d/%d\n", succeeded, suite->length);
    }
    testsuite_t *next = suite->next;
    succeeded = 0;
    free(suite);
    suite_list.first = next;
  }
  return (tunit_total_errors != 0);
}

void t_addTestToSuite(char *name, void (*test_fn)(void *)) {
  test_t *t = (test_t *)malloc(sizeof(test_t));
  t->test_fn = test_fn;
  t->next = suite_list.first->first;
  t->name = name;
  suite_list.first->first = t;
  suite_list.first->length++;
}

#endif
