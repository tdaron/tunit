#include <stdio.h>
#include <stdlib.h>
#ifndef TUNIT_H
#define TUNIT_H 42
typedef struct TestSuite testsuite_t;
typedef struct Test test_t;
/*
Definitions of available methods.

*/

testsuite_t *t_registerTestSuite(char *name);
test_t *t_addTestToSuite(testsuite_t *suite, char *name,
                         void (*test_fn)(void *));
void t_addStaticDataToTest(test_t *test, void *data);
void t_addStartUpToTest(test_t *test, void (*d)(void *));
void t_addCleanUpToTest(test_t *test, void (*d)(void *));
int t_runSuites(int argc, char **argv);

/*
Definitions of macros and constants

*/
#define C_NORM "\033[0m"
#define C_RED "\033[0;31m"
#define C_GREEN "\033[0;32m"

#define t_errf(a, op, b)                                                       \
  snprintf(tunit_error_string, tunit_error_string_length,                      \
           C_RED "FAIL> %s:%d - assertion failed %d %s %d\n" C_NORM " ",   \
           __FILE__, __LINE__, a, #op, b);

#define t_assert_int(a, op, b)                                                 \
  if (!((a)op(b))) {                                                           \
    tunit_error = 1;                                                           \
    int length = t_errf(a, op, b);                                             \
    /* in this case we must allocate more memory so store error message*/      \
    if (length > tunit_error_string_length) {                                  \
      free(tunit_error_string);                                                \
      tunit_error_string = malloc(length + 1);                                 \
      tunit_error_string_length = length;                                      \
      t_errf(a, op, b);                                                        \
    }                                                                          \
  }
#define t_assert_false(a) t_assert_int(a, ==, 0)
#define t_assert_true(a) t_assert_int(a, ==, 1)
#endif
//TODO: Remove this define. Currently useful for IDE.
#define TUNIT_IMPLEMENTATION
/*
Implementation of methods.

*/
#ifdef TUNIT_IMPLEMENTATION

// Global variables
int succeeded;
int tunit_error;
int tunit_total_errors = 0;
char *tunit_error_string = NULL;
unsigned long tunit_error_string_length = 0; // usefull to not allocate memory.
typedef struct Test test_t;                  // forward declaration
struct Test {
  test_t *next;
  char *name;
  void (*test_fn)(void *);
  void (*start_up)(void *);
  void (*clean_up)(void *);
  void *static_data;
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
testsuite_t *t_registerTestSuite(char *name) {
  testsuite_t *new_suite = (testsuite_t *)malloc(sizeof(testsuite_t));
  new_suite->name = name;
  new_suite->first = NULL;
  new_suite->next = suite_list.first;
  suite_list.first = new_suite;
  suite_list.length++;
  return suite_list.first;
}

void pv_t_runSuite(testsuite_t *t) {
  while (t->first != NULL) {
    test_t *test = t->first;
    void *input = test->static_data;
    if (test->start_up != NULL) {
      test->start_up(input);
    }
    test->test_fn(input);
    if (test->clean_up != NULL) {
      test->clean_up(input);
    }
    if (tunit_error == 0) {
      succeeded++;
      printf("\t--> " C_GREEN "%s\n" C_NORM, test->name);
    } else {
      tunit_total_errors = 1;
      printf("\t--> " C_RED "%s\n" C_NORM, test->name);
      printf("%s", tunit_error_string);
    }
    tunit_error = 0;
    t->first = test->next;
    free(test);
  }
  printf("\n");
}

int t_runSuites(int argc, char **argv) {
  while (suite_list.first != NULL) {
    testsuite_t *suite = suite_list.first;
    if (suite->length == 0) {
      printf("Skipping %s because not tests in here\n", suite->name);
    } else {
      printf("\n---------------\n\n");
      printf("Running %s\n", suite->name);
      pv_t_runSuite(suite);
      printf("Succeeded " C_GREEN "%d/%d" C_NORM, succeeded, suite->length);
      if (succeeded != suite->length) {
        printf(" - Failed" C_RED " %d/%d" C_NORM, suite->length - succeeded,
               suite->length);
      }
      printf("\n");
    }
    testsuite_t *next = suite->next;
    succeeded = 0;
    free(suite);
    suite_list.first = next;
  }
  if (tunit_error_string != NULL) {
    free(tunit_error_string);
  }
  return (tunit_total_errors != 0);
}

void t_addStaticDataToTest(test_t *test, void *data) {
  test->static_data = data;
}

test_t *t_addTestToSuite(testsuite_t *suite, char *name,
                         void (*test_fn)(void *)) {
  test_t *t = (test_t *)malloc(sizeof(test_t));
  t->test_fn = test_fn;
  t->next = suite->first;
  t->name = name;
  t->start_up = NULL;
  t->clean_up = NULL;
  t->static_data = NULL;
  suite->first = t;
  suite->length++;
  free(tunit_error_string);
  return t;
}
void t_addStartUpToTest(test_t *test, void (*startup)(void *)) {
  test->start_up = startup;
}
void t_addCleanUpToTest(test_t *test, void (*cleanup)(void *)) {
  test->clean_up = cleanup;
}

#endif
