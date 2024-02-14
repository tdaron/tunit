#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
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
  fprintf(stderr,                                                              \
          C_RED "FAIL> %s(%s):%d - assertion failed %d %s %d" C_NORM "\n",        \
          __FILE__, __func__,  __LINE__, a, #op, b);

#define t_assert_int(a, op, b)                                                 \
  if (!((a)op(b))) {                                                           \
    t_errf(a, op, b);                                                          \
  }
#define t_assert_false(a) t_assert_int(a, ==, 0)
#define t_assert_true(a) t_assert_int(a, ==, 1)
#endif
// TODO: Remove this define. Currently useful for IDE.
#define TUNIT_IMPLEMENTATION
/*
Implementation of methods.

*/
#ifdef TUNIT_IMPLEMENTATION

// Global variables
typedef struct Test test_t; // forward declaration
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


static char * getContent(FILE * file, int length) {
    rewind(file);
    char * output = (char*)malloc(length+10); //+10 is a margin of 'safety'
    fgets(output, length+10, file);
    return output;
}

static int pv_t_runTest(test_t *test) {

  FILE *new_stderr = tmpfile();
  FILE *new_stdout = tmpfile();
  pid_t pid = fork();

  // here we are inside the fork
  if (pid == 0) {
    dup2(fileno(new_stderr), STDERR_FILENO);
    dup2(fileno(new_stdout), STDOUT_FILENO);

    void *input = test->static_data;
    if (test->start_up != NULL) {
      test->start_up(input);
    }
    test->test_fn(input);
    if (test->clean_up != NULL) {
      test->clean_up(input);
    }
    exit(0);
  }
  int status = 0;
  waitpid(pid, &status, 0);
  long int stderr_length = ftell(new_stderr);
  char * error_output = NULL;
  char * output = NULL;
  int error = status != 0 || stderr_length > 0;
  if (error) {
    int stdout_length = ftell(new_stdout);
    error_output = getContent(new_stderr, stderr_length);
    output = getContent(new_stdout, stdout_length);
  }
  char * color = error ? C_RED : C_GREEN;
  printf("\t-> %s%s\n" C_NORM, color, test->name);
  if (output != NULL) {
    printf("%s", output);
  }
  if (error_output != NULL) {
    printf("%s", error_output);
  }
  close(fileno(new_stdout));
  close(fileno(new_stderr));
  return error ? 1 : 0;
}

static void pv_t_runSuite(testsuite_t *suite) {
  printf("\n---------------\n\n");
  printf("Running %s\n", suite->name);
  size_t failed = 0;
  while (suite->first != NULL) {
    test_t *test = suite->first;
    suite->first = test->next;
    int res = pv_t_runTest(test);
    failed += res; //1 if error and 0 if not.
    free(test);
  }
  printf("\nSucceeded " C_GREEN "%ld/%ld" C_NORM, suite->length-failed, suite->length);
  if (failed > 0) {
    printf(" - Failed" C_RED " %ld/%ld" C_NORM, failed,
           suite->length);
  }
  printf("\n");
}

int t_runSuites(int argc, char **argv) {
  while (suite_list.first != NULL) {
    testsuite_t *suite = suite_list.first;
    if (suite->length == 0) {
      printf("Skipping %s because not tests in here\n", suite->name);
    } else {
      pv_t_runSuite(suite);
    }
    testsuite_t *next = suite->next;
    free(suite);
    suite_list.first = next;
  }
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
  return t;
}
void t_addStartUpToTest(test_t *test, void (*startup)(void *)) {
  test->start_up = startup;
}
void t_addCleanUpToTest(test_t *test, void (*cleanup)(void *)) {
  test->clean_up = cleanup;
}

#endif
