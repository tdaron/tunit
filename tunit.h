#include <stdio.h>
#include <stdlib.h>
#ifndef TUNIT_H
#define TUNIT_H 42
typedef struct TestSuite testsuite_t;
typedef struct Test test_t;
testsuite_t *t_registerTestSuite(char *name);
test_t *t_addTestToSuite(testsuite_t *suite, char *name,
                         void (*test_fn)(void *));
void t_addStaticDataToTest(test_t *test, void *data);
int t_runSuites(int argc, char **argv);
#define C_NORM "\033[0m"
#define C_RED "\033[0;31m"
#define C_GREEN "\033[0;32m"
#define C_YELLOW "\033[0;33m"
#define C_BLUE "\033[0;34m"
#define C_MAGENTA "\033[0;35m"
#define C_CYAN "\033[0;36m"

#define t_assert_int(a, op, b)                                                 \
  if (!((a)op(b))) {                                                           \
    tunit_error = 1;                                                           \
    sprintf(tunit_error_string,                                                \
            C_RED "ERROR> %s:%d - assertion failed %d %s %d\n" C_NORM,         \
            __FILE__, __LINE__, a, #op, b);                                    \
  }

#endif
#ifdef TUNIT_IMPLEMENTATION
typedef struct Test test_t; // forward declaration
struct Test {
  test_t *next;
  char *name;
  void (*test_fn)(void *);
  void *(*start_up)();
  void (*clean_up)();
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
int succeeded;
int tunit_error;
int tunit_total_errors = 0;
char *tunit_error_string;
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
      input = test->start_up();
    }
    test->test_fn(input);
    if (test->clean_up != NULL) {
      test->clean_up();
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
  return (tunit_total_errors != 0);
}

void t_addStaticDataToTest(test_t * test, void *data) {
  test->static_data = data;
}


test_t *t_addTestToSuite(testsuite_t *suite, char *name,
                         void (*test_fn)(void *)) {
  // TODO: Find a better way to do this lul
  tunit_error_string = malloc(1000);
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

#endif
