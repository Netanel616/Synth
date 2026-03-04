/*
 * Acutest -- A simple C/C++ unit testing framework
 *
 * Copyright 2014-2021, 2023-2024 Martin Mitas
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ACUTEST_H__
#define ACUTEST_H__

/*
 * Public Acutest interface
 */

/**
 * @defgroup public Public interface
 * @{
 */

/**
 * @brief Mark a function as a test unit.
 *
 * The function shall have no parameters and shall not return any value
 * (`void foo(void)`).
 *
 * This is just a convenience macro which may be used to make test units more
 * visible in the source code.
 */
#define TEST_CASE(func)      void func(void)

/**
 * @brief List of test units.
 *
 * The list is an array of ::TEST_UNIT structures, terminated with
 * `{ NULL, NULL }`.
 *
 * @see TEST_UNIT
 */
#define TEST_LIST      const struct test_unit_ test_list_[]

/**
 * @brief Test unit structure.
 *
 * The structure holds a pointer to the test function and its name.
 *
 * @see TEST_LIST
 */
#define TEST_UNIT      struct test_unit_

/**
 * @brief Check a condition.
 *
 * If the condition is false, the check fails and the test unit continues.
 *
 * @param cond  Condition to be checked.
 */
#define TEST_CHECK(cond) \
    do { \
        acutest_check_((cond), __FILE__, __LINE__, #cond); \
    } while(0)

/**
 * @brief Check a condition.
 *
 * If the condition is false, the check fails and the test unit is aborted.
 *
 * @param cond  Condition to be checked.
 */
#define TEST_ASSERT(cond) \
    do { \
        if(!acutest_check_((cond), __FILE__, __LINE__, #cond)) \
            return; \
    } while(0)

/**
 * @brief Unconditionally fail.
 *
 * This check always fails and the test unit continues.
 *
 * @param msg   A message to be displayed. It has to be a string literal.
 */
#define TEST_FAIL(msg) \
    do { \
        acutest_check_(0, __FILE__, __LINE__, (msg)); \
    } while(0)

/**
 * @brief Unconditionally fail.
 *
 * This check always fails and the test unit is aborted.
 *
 * @param msg   A message to be displayed. It has to be a string literal.
 */
#define TEST_ABORT(msg) \
    do { \
        acutest_check_(0, __FILE__, __LINE__, (msg)); \
        return; \
    } while(0)

/**
 * @brief Check for an exception.
 *
 * This is for C++ tests only. The check fails if the code does not throw
 * an exception of the given type.
 *
 * The check is non-aborting.
 *
 * @param code  The code which is expected to throw the exception.
 * @param ex    The expected exception type.
 */
#ifdef __cplusplus
    #define TEST_EXCEPTION(code, ex) \
        do { \
            acutest_check_exception_(__FILE__, __LINE__, #code, #ex, \
                                     [&]() { code; }); \
        } while(0)
#endif

/**
 * @brief Check for an exception.
 *
 * This is for C++ tests only. The check fails if the code does not throw
 * an exception of the given type.
 *
 * The check is aborting.
 *
 * @param code  The code which is expected to throw the exception.
 * @param ex    The expected exception type.
 */
#ifdef __cplusplus
    #define TEST_EXCEPTION_ASSERT(code, ex) \
        do { \
            if(!acutest_check_exception_(__FILE__, __LINE__, #code, #ex, \
                                         [&]() { code; })) \
                return; \
        } while(0)
#endif

/**
 * @brief Check for a C++-style exception.
 *
 * This is for C++ tests only. The check fails if the code does not throw
 * any C++-style exception.
 *
 * The check is non-aborting.
 *
 * @param code  The code which is expected to throw the exception.
 */
#ifdef __cplusplus
    #define TEST_ANY_EXCEPTION(code) \
        do { \
            acutest_check_any_exception_(__FILE__, __LINE__, #code, \
                                         [&]() { code; }); \
        } while(0)
#endif

/**
 * @brief Check for a C++-style exception.
 *
 * This is for C++ tests only. The check fails if the code does not throw
 * any C++-style exception.
 *
 * The check is aborting.
 *
 * @param code  The code which is expected to throw the exception.
 */
#ifdef __cplusplus
    #define TEST_ANY_EXCEPTION_ASSERT(code) \
        do { \
            if(!acutest_check_any_exception_(__FILE__, __LINE__, #code, \
                                             [&]() { code; })) \
                return; \
        } while(0)
#endif

/**
 * @brief A mock of a function.
 *
 * This can be used to check whether a function is called, how many times it
 * is called, and to specify its return value.
 *
 * To use it, you have to:
 *  - Define a real function with a special name `real_...()`.
 *  - Redefine the function as a mock with `TEST_MOCK_...`.
 *
 * Example:
 * @code
 * // Real function for non-test builds.
 * int real_foo(int a, int b);
 *
 * // In the test, we can mock it.
 * TEST_MOCK_V(int, foo, (int a, int b), (a, b), { ... }, 0);
 *
 * // Now, `foo()` is a mock. The real implementation is available as `real_foo()`.
 * // The mock can be controlled with `TEST_MOCK_..._SETUP`.
 * @endcode
 *
 * @param type      Return type of the function.
 * @param name      Name of the function.
 * @param args_decl Declaration of function arguments, including parentheses.
 * @param args_pass Arguments to pass to the real function, including parentheses.
 * @param body      Body of the mock function.
 * @param retval    Value to be returned by the mock if not specified otherwise
 *                  by `TEST_MOCK_..._SETUP_RETURN()`.
 */
#define TEST_MOCK_V(type, name, args_decl, args_pass, body, retval) \
    struct test_mock_ { \
        long count; \
        type (*func) args_decl; \
        type retval; \
    } test_mock_##name##_ = { 0, NULL, (retval) }; \
    type name args_decl \
    { \
        test_mock_##name##_.count++; \
        if(test_mock_##name##_.func != NULL) \
            return test_mock_##name##_.func args_pass; \
        body; \
        return test_mock_##name##_.retval; \
    }

/**
 * @brief A mock of a function with `void` return type.
 *
 * This is a variant of `TEST_MOCK_V()` for functions returning `void`.
 *
 * @param name      Name of the function.
 * @param args_decl Declaration of function arguments, including parentheses.
 * @param args_pass Arguments to pass to the real function, including parentheses.
 * @param body      Body of the mock function.
 */
#define TEST_MOCK_VOID(name, args_decl, args_pass, body) \
    struct test_mock_ { \
        long count; \
        void (*func) args_decl; \
    } test_mock_##name##_ = { 0, NULL }; \
    void name args_decl \
    { \
        test_mock_##name##_.count++; \
        if(test_mock_##name##_.func != NULL) { \
            test_mock_##name##_.func args_pass; \
            return; \
        } \
        body; \
    }

/**
 * @brief Setup a mock.
 *
 * This allows to control behavior of a mock created with `TEST_MOCK_V()` or
 * `TEST_MOCK_VOID()`.
 *
 * @param name      Name of the mocked function.
 */
#define TEST_MOCK_SETUP(name) \
    test_mock_##name##_

/**
 * @brief Reset a mock to its initial state.
 *
 * @param name      Name of the mocked function.
 */
#define TEST_MOCK_RESET(name) \
    do { \
        test_mock_##name##_.count = 0; \
        test_mock_##name##_.func = NULL; \
    } while(0)

/**
 * @brief Tell a mock to call the real implementation of the function.
 *
 * @param name      Name of the mocked function.
 */
#define TEST_MOCK_CALL_REAL(name) \
    (test_mock_##name##_.func = real_##name)

/**
 * @brief Tell a mock to call a custom implementation of the function.
 *
 * @param name      Name of the mocked function.
 * @param func      The custom implementation.
 */
#define TEST_MOCK_CALL_CUSTOM(name, func) \
    (test_mock_##name##_.func = (func))

/**
 * @brief Tell a mock to return a specific value.
 *
 * This is for mocks created with `TEST_MOCK_V()` only.
 *
 * @param name      Name of the mocked function.
 * @param value     The value to be returned.
 */
#define TEST_MOCK_SET_RETURN(name, value) \
    (test_mock_##name##_.retval = (value))

/**
 * @brief Get how many times the mock has been called.
 *
 * @param name      Name of the mocked function.
 */
#define TEST_MOCK_COUNT(name) \
    (test_mock_##name##_.count)

/**
 * @brief Dump a memory block.
 *
 * The memory block is printed as a hexadecimal dump to the standard output.
 * This may be useful for debugging.
 *
 * @param mem       Pointer to the memory block.
 * @param size      Size of the memory block.
 */
#define TEST_DUMP(mem, size) \
    acutest_dump_(__FILE__, __LINE__, (mem), (size))

/**
 * @brief Initialize the test unit.
 *
 * This is called before the test unit is executed. This is a good place for
 * any initialization.
 *
 * This is optional. If you do not need any initialization, you do not need
 * to implement this function.
 *
 * The function shall have no parameters and shall not return any value
 * (`void test_init(void)`).
 */
#define TEST_INIT(func)      void func(void)

/**
 * @brief De-initialize the test unit.
 *
 * This is called after the test unit is executed. This is a good place for
 * any clean-up.
 *
 * This is optional. If you do not need any clean-up, you do not need
 * to implement this function.
 *
 * The function shall have no parameters and shall not return any value
 * (`void test_fini(void)`).
 */
#define TEST_FINI(func)      void func(void)

/**
 * @}
 */


/*
 * Implementation details
 */

#include <ctype.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
    #include <exception>
    #include <functional>
#endif

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__)
    #include <errno.h>
    #include <libgen.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #if defined(__APPLE__)
        #include <mach-o/dyld.h>
    #endif
    #define ACUTEST_UNIX_
#endif

#if defined(_WIN32) || defined(__WIN32__)
    #include <windows.h>
    #include <io.h>
    #define ACUTEST_WIN_
#endif

#if !defined(ACUTEST_NO_THREADS_)
    #if defined(ACUTEST_WIN_)
        /* Minimal thread support for Windows. */
        typedef HANDLE                acutest_thread_t;
        typedef DWORD                 acutest_thread_id_t;
        #define ACUTEST_THREAD_NULL   NULL
    #elif defined(ACUTEST_UNIX_)
        /* Minimal thread support for POSIX. */
        #include <pthread.h>
        typedef pthread_t             acutest_thread_t;
        typedef pthread_t             acutest_thread_id_t;
        #define ACUTEST_THREAD_NULL   0
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif


struct test_unit_ {
    void (*func)(void);
    const char* name;
};


/* Global state of the test suite. */
struct test_ {
    /* Counters of failed and passed checks. */
    int failed_checks;
    int passed_checks;

    /* Counters of failed and passed test units. */
    int failed_units;
    int passed_units;

    /* Counter of all test units. */
    int total_units;

    /* Pointer to the current test unit. */
    const struct test_unit_* current_unit;

    /* Jump buffer for skipping to the end of the current test unit. */
    jmp_buf jump;

    /* Command line arguments. */
    int argc;
    char** argv;

    /* Name of the executable. */
    char* progname;

    /* Options from command line. */
    int list_units;
    int no_exec;
    int no_summary;
    int no_colors;
    int silent;
    int verbose_level;
    const char* tap;
    const char** unit_names;
    int unit_names_count;

#if !defined(ACUTEST_NO_FORK_) && defined(ACUTEST_UNIX_)
    /* PID of the child process executing the current test unit. */
    pid_t child_pid;
#endif

#if !defined(ACUTEST_NO_THREADS_)
    /* Minimalist thread support. */
    acutest_thread_t main_thread;
    acutest_thread_id_t main_thread_id;
#endif
};

extern struct test_ test_;


int acutest_check_(int cond, const char* file, int line, const char* text);
void acutest_dump_(const char* file, int line,
                   const void* mem, size_t size);

#ifdef __cplusplus
    bool acutest_check_exception_(const char* file, int line,
                                  const char* code, const char* ex_name,
                                  const std::function<void()>& func);
    bool acutest_check_any_exception_(const char* file, int line,
                                      const char* code,
                                      const std::function<void()>& func);
#endif


#if defined(ACUTEST_IMPLEMENTATION_) || defined(TEST_NO_MAIN)


/*
 * Pre-defined colors for internal usage.
 *
 * We use them only if we are writing to a terminal.
 */
#define COLOR_RED       "\033[31m"
#define COLOR_GREEN     "\033[32m"
#define COLOR_YELLOW    "\033[33m"
#define COLOR_DEFAULT   "\033[0m"

static const char* color_red_ = "";
static const char* color_green_ = "";
static const char* color_yellow_ = "";
static const char* color_default_ = "";


/* Output: We buffer the output from the test so that we can redirect it to
 * a TAP file if requested. */

static char* output_buffer_ = NULL;
static size_t output_buffer_size_ = 0;
static size_t output_buffer_capacity_ = 0;

static void
output_init_(void)
{
    output_buffer_capacity_ = 1024;
    output_buffer_ = (char*) malloc(output_buffer_capacity_);
    if(output_buffer_ == NULL) {
        fprintf(stderr, "error: out of memory.\n");
        exit(1);
    }
    output_buffer_size_ = 0;
}

static void
output_destroy_(void)
{
    free(output_buffer_);
}

static void
output_flush_(void)
{
    if(output_buffer_size_ == 0)
        return;

    if(test_.tap != NULL) {
        FILE* f = fopen(test_.tap, "a");
        if(f == NULL) {
            fprintf(stderr, "error: cannot open TAP file '%s'.\n", test_.tap);
            exit(1);
        }
        fwrite(output_buffer_, 1, output_buffer_size_, f);
        fclose(f);
    } else {
        fwrite(output_buffer_, 1, output_buffer_size_, stdout);
        fflush(stdout);
    }

    output_buffer_size_ = 0;
}

static void
output_vprintf_(const char* fmt, va_list args)
{
    va_list args_copy;
    int n;

    va_copy(args_copy, args);
    n = vsnprintf(output_buffer_ + output_buffer_size_,
                  output_buffer_capacity_ - output_buffer_size_,
                  fmt, args_copy);
    va_end(args_copy);

    if(output_buffer_size_ + n >= output_buffer_capacity_) {
        size_t new_capacity = output_buffer_capacity_ * 2;
        if(new_capacity < output_buffer_size_ + n + 1)
            new_capacity = output_buffer_size_ + n + 1;
        output_buffer_ = (char*) realloc(output_buffer_, new_capacity);
        if(output_buffer_ == NULL) {
            fprintf(stderr, "error: out of memory.\n");
            exit(1);
        }
        output_buffer_capacity_ = new_capacity;

        va_copy(args_copy, args);
        n = vsnprintf(output_buffer_ + output_buffer_size_,
                      output_buffer_capacity_ - output_buffer_size_,
                      fmt, args_copy);
        va_end(args_copy);
    }

    output_buffer_size_ += n;
}

static void
output_printf_(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    output_vprintf_(fmt, args);
    va_end(args);
}

static void
output_puts_(const char* s)
{
    output_printf_("%s\n", s);
}


/* Utilities */

static const char*
short_filename_(const char* filename)
{
    const char* short_name = filename;

#ifdef ACUTEST_WIN_
    /* We cannot use dirname() from <libgen.h> as it is not available on
     * Windows. We have to parse the path manually. */
    const char* p = filename;
    while(*p != '\0') {
        if(*p == '/' || *p == '\\')
            short_name = p + 1;
        p++;
    }
#else
    /* We can use POSIX basename(). However, we need to be careful as it
     * may modify the input string. */
    char* filename_copy = strdup(filename);
    if(filename_copy == NULL) {
        fprintf(stderr, "error: out of memory.\n");
        exit(1);
    }
    short_name = basename(filename_copy);
    if(strcmp(filename, short_name) != 0) {
        /* The path was modified. We have to copy the result. */
        filename = strdup(short_name);
    }
    free(filename_copy);
#endif

    return short_name;
}

static void
print_progress_(void)
{
    if(test_.silent)
        return;

    if(test_.tap != NULL)
        return;

    output_printf_("Test %d/%d (%s): ",
                   test_.passed_units + test_.failed_units + 1,
                   test_.total_units,
                   test_.current_unit->name);
    output_flush_();
}

static void
print_summary_(void)
{
    if(test_.no_summary)
        return;

    if(test_.tap != NULL) {
        output_printf_("1..%d\n", test_.total_units);
    } else {
        output_printf_("\n");
        output_printf_("Summary:\n");
        output_printf_("  Test units: %d total, %d passed, %d failed\n",
                       test_.passed_units + test_.failed_units,
                       test_.passed_units,
                       test_.failed_units);
        output_printf_("  Checks:     %d total, %d passed, %d failed\n",
                       test_.passed_checks + test_.failed_checks,
                       test_.passed_checks,
                       test_.failed_checks);
    }
    output_flush_();
}

static void
print_unit_list_(const struct test_unit_* test_list)
{
    const struct test_unit_* unit;

    output_printf_("Available test units:\n");
    for(unit = test_list; unit->func != NULL; unit++)
        output_printf_("  %s\n", unit->name);
    output_flush_();
}

static void
parse_args_(int argc, char** argv)
{
    int i;

    test_.argc = argc;
    test_.argv = argv;

    /* Default values. */
    test_.list_units = 0;
    test_.no_exec = 0;
    test_.no_summary = 0;
    test_.no_colors = 0;
    test_.silent = 0;
    test_.verbose_level = 1;
    test_.tap = NULL;
    test_.unit_names = NULL;
    test_.unit_names_count = 0;

    /* The executable name. */
#if defined(ACUTEST_WIN_)
    char progname_buff[MAX_PATH];
    if(GetModuleFileNameA(NULL, progname_buff, sizeof(progname_buff)) == 0) {
        fprintf(stderr, "error: cannot get executable name.\n");
        exit(1);
    }
    test_.progname = strdup(progname_buff);
#elif defined(__APPLE__)
    char progname_buff[1024];
    uint32_t progname_buff_size = sizeof(progname_buff);
    if(_NSGetExecutablePath(progname_buff, &progname_buff_size) != 0) {
        fprintf(stderr, "error: cannot get executable name.\n");
        exit(1);
    }
    test_.progname = strdup(progname_buff);
#elif defined(ACUTEST_UNIX_)
    char progname_buff[1024];
    if(readlink("/proc/self/exe", progname_buff, sizeof(progname_buff)) < 0) {
        /* This can fail (e.g. on FreeBSD). Fallback to argv[0]. */
        test_.progname = strdup(argv[0]);
    } else {
        test_.progname = strdup(progname_buff);
    }
#else
    test_.progname = strdup(argv[0]);
#endif

    /* For compatibility with older compilers, we do not use getopt(). */
    for(i = 1; i < argc; i++) {
        if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Usage: %s [options] [test-unit-name...]\n", argv[0]);
            printf("Options:\n");
            printf("  --help, -h         Show this help message and exit.\n");
            printf("  --list, -l         List available test units and exit.\n");
            printf("  --no-exec, -n      Do not execute the tests.\n");
            printf("  --no-summary       Do not show the summary.\n");
            printf("  --no-colors        Do not use colors.\n");
            printf("  --silent, -s       Do not print any output unless a check fails.\n");
            printf("  --verbose, -v      Be more verbose. Can be used multiple times.\n");
            printf("  --tap=FILE         Redirect test results in TAP format to a file.\n");
            exit(0);
        } else if(strcmp(argv[i], "--list") == 0 || strcmp(argv[i], "-l") == 0) {
            test_.list_units = 1;
        } else if(strcmp(argv[i], "--no-exec") == 0 || strcmp(argv[i], "-n") == 0) {
            test_.no_exec = 1;
        } else if(strcmp(argv[i], "--no-summary") == 0) {
            test_.no_summary = 1;
        } else if(strcmp(argv[i], "--no-colors") == 0) {
            test_.no_colors = 1;
        } else if(strcmp(argv[i], "--silent") == 0 || strcmp(argv[i], "-s") == 0) {
            test_.silent = 1;
        } else if(strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
            test_.verbose_level++;
        } else if(strncmp(argv[i], "--tap=", 6) == 0) {
            test_.tap = argv[i] + 6;
        } else if(argv[i][0] == '-') {
            fprintf(stderr, "error: unknown option '%s'.\n", argv[i]);
            exit(1);
        } else {
            test_.unit_names = (const char**) &argv[i];
            test_.unit_names_count = argc - i;
            break;
        }
    }
}

static int
should_run_(const char* name)
{
    int i;

    if(test_.unit_names_count == 0)
        return 1;

    for(i = 0; i < test_.unit_names_count; i++) {
        if(strcmp(name, test_.unit_names[i]) == 0)
            return 1;
    }

    return 0;
}

static void
setup_colors_(void)
{
    int is_a_tty = 0;

#ifdef ACUTEST_WIN_
    is_a_tty = _isatty(_fileno(stdout));
#else
    is_a_tty = isatty(fileno(stdout));
#endif

    if(test_.no_colors || !is_a_tty)
        return;

    color_red_ = COLOR_RED;
    color_green_ = COLOR_GREEN;
    color_yellow_ = COLOR_YELLOW;
    color_default_ = COLOR_DEFAULT;
}

static void
run_test_unit_(const struct test_unit_* unit)
{
    test_.current_unit = unit;
    test_.failed_checks = 0;
    test_.passed_checks = 0;

    /* This is a good place to call any global initialization. */
    void test_init(void);
    if(&test_init)
        test_init();

    /* If the test unit aborts, it will jump here. */
    if(setjmp(test_.jump) == 0)
        unit->func();

    /* This is a good place to call any global de-initialization. */
    void test_fini(void);
    if(&test_fini)
        test_fini();

    if(test_.failed_checks == 0) {
        test_.passed_units++;
        if(!test_.silent) {
            if(test_.tap != NULL) {
                output_printf_("ok %d - %s\n",
                               test_.passed_units + test_.failed_units,
                               unit->name);
            } else {
                output_printf_("%sPASSED%s\n", color_green_, color_default_);
            }
        }
    } else {
        test_.failed_units++;
        if(!test_.silent) {
            if(test_.tap != NULL) {
                output_printf_("not ok %d - %s\n",
                               test_.passed_units + test_.failed_units,
                               unit->name);
            } else {
                output_printf_("%sFAILED%s\n", color_red_, color_default_);
            }
        }
    }
    output_flush_();
}

#if !defined(ACUTEST_NO_FORK_) && defined(ACUTEST_UNIX_)
static void
run_test_unit_forked_(const struct test_unit_* unit)
{
    int status;

    test_.child_pid = fork();
    if(test_.child_pid < 0) {
        fprintf(stderr, "error: fork() failed: %s\n", strerror(errno));
        exit(1);
    }

    if(test_.child_pid == 0) {
        /* Child: Execute the test unit. */
        run_test_unit_(unit);
        exit(0);
    }

    /* Parent: Wait for the child to finish. */
    if(waitpid(test_.child_pid, &status, 0) < 0) {
        fprintf(stderr, "error: waitpid() failed: %s\n", strerror(errno));
        exit(1);
    }

    /* Check how the child has finished. */
    if(WIFEXITED(status)) {
        if(WEXITSTATUS(status) != 0) {
            test_.failed_units++;
            if(!test_.silent) {
                if(test_.tap != NULL) {
                    output_printf_("not ok %d - %s\n",
                                   test_.passed_units + test_.failed_units,
                                   unit->name);
                } else {
                    output_printf_("%sCRASHED%s\n", color_red_, color_default_);
                }
            }
        }
    } else if(WIFSIGNALED(status)) {
        test_.failed_units++;
        if(!test_.silent) {
            if(test_.tap != NULL) {
                output_printf_("not ok %d - %s\n",
                               test_.passed_units + test_.failed_units,
                               unit->name);
            } else {
                output_printf_("%sCRASHED%s (signal %d)\n",
                               color_red_, color_default_, WTERMSIG(status));
            }
        }
    }
}
#endif

int
acutest_check_(int cond, const char* file, int line, const char* text)
{
    if(cond) {
        test_.passed_checks++;
    } else {
        test_.failed_checks++;
        if(!test_.silent) {
            if(test_.tap != NULL) {
                output_printf_("#   Error: %s:%d: %s\n", short_filename_(file), line, text);
            } else {
                output_printf_("\n  %sError: %s:%d: %s%s\n",
                               color_yellow_, short_filename_(file), line, text,
                               color_default_);
            }
        }
    }
    return cond;
}

void
acutest_dump_(const char* file, int line,
              const void* mem, size_t size)
{
    const unsigned char* p = (const unsigned char*) mem;
    size_t i;

    if(test_.silent)
        return;

    if(test_.tap != NULL)
        output_printf_("#   Dump: %s:%d: %zu bytes at %p\n",
                       short_filename_(file), line, size, mem);
    else
        output_printf_("\n  %sDump: %s:%d: %zu bytes at %p%s\n",
                       color_yellow_, short_filename_(file), line, size, mem,
                       color_default_);

    for(i = 0; i < size; i += 16) {
        size_t j;

        if(test_.tap != NULL)
            output_printf_("#     %08zx: ", i);
        else
            output_printf_("    %08zx: ", i);

        for(j = 0; j < 16; j++) {
            if(i + j < size)
                output_printf_("%02x ", p[i+j]);
            else
                output_printf_("   ");
        }

        output_printf_(" ");

        for(j = 0; j < 16; j++) {
            if(i + j < size)
                output_printf_("%c", isprint(p[i+j]) ? p[i+j] : '.');
        }

        output_printf_("\n");
    }
}

#ifdef __cplusplus
bool
acutest_check_exception_(const char* file, int line,
                         const char* code, const char* ex_name,
                         const std::function<void()>& func)
{
    try {
        func();
    } catch(const std::exception& e) {
        if(typeid(e).name() == ex_name) {
            test_.passed_checks++;
            return true;
        }
    } catch(...) {
        /* Ignore other exceptions. */
    }

    test_.failed_checks++;
    if(!test_.silent) {
        if(test_.tap != NULL) {
            output_printf_("#   Error: %s:%d: %s: Expected exception %s not thrown.\n",
                           short_filename_(file), line, code, ex_name);
        } else {
            output_printf_("\n  %sError: %s:%d: %s: Expected exception %s not thrown.%s\n",
                           color_yellow_, short_filename_(file), line, code, ex_name,
                           color_default_);
        }
    }
    return false;
}

bool
acutest_check_any_exception_(const char* file, int line,
                             const char* code,
                             const std::function<void()>& func)
{
    try {
        func();
    } catch(...) {
        test_.passed_checks++;
        return true;
    }

    test_.failed_checks++;
    if(!test_.silent) {
        if(test_.tap != NULL) {
            output_printf_("#   Error: %s:%d: %s: Expected exception not thrown.\n",
                           short_filename_(file), line, code);
        } else {
            output_printf_("\n  %sError: %s:%d: %s: Expected exception not thrown.%s\n",
                           color_yellow_, short_filename_(file), line, code,
                           color_default_);
        }
    }
    return false;
}
#endif

#if !defined(ACUTEST_NO_THREADS_)
static acutest_thread_id_t
acutest_thread_id_self_(void)
{
#if defined(ACUTEST_WIN_)
    return GetCurrentThreadId();
#elif defined(ACUTEST_UNIX_)
    return pthread_self();
#else
    return 0;
#endif
}

static int
acutest_thread_id_equal_(acutest_thread_id_t a, acutest_thread_id_t b)
{
#if defined(ACUTEST_WIN_)
    return (a == b);
#elif defined(ACUTEST_UNIX_)
    return pthread_equal(a, b);
#else
    return 1;
#endif
}
#endif


/* Global state of the test suite. */
struct test_ test_;


/* We provide empty stubs for test_init() and test_fini() if the user does
 * not provide them. */
void test_init(void) __attribute__((weak));
void test_init(void) { }
void test_fini(void) __attribute__((weak));
void test_fini(void) { }


#ifndef TEST_NO_MAIN

extern const struct test_unit_ test_list_[];

int
main(int argc, char** argv)
{
    const struct test_unit_* unit;

    output_init_();
    parse_args_(argc, argv);
    setup_colors_();

#if !defined(ACUTEST_NO_THREADS_)
    test_.main_thread = ACUTEST_THREAD_NULL;
    test_.main_thread_id = acutest_thread_id_self_();
#endif

    if(test_.list_units) {
        print_unit_list_(test_list_);
        goto end;
    }

    if(test_.no_exec)
        goto end;

    if(test_.tap != NULL) {
        /* Erase the TAP file. */
        FILE* f = fopen(test_.tap, "w");
        if(f == NULL) {
            fprintf(stderr, "error: cannot open TAP file '%s'.\n", test_.tap);
            exit(1);
        }
        fclose(f);
    }

    for(unit = test_list_; unit->func != NULL; unit++) {
        if(should_run_(unit->name))
            test_.total_units++;
    }

    for(unit = test_list_; unit->func != NULL; unit++) {
        if(!should_run_(unit->name))
            continue;

        print_progress_();

#if !defined(ACUTEST_NO_FORK_) && defined(ACUTEST_UNIX_)
        if(test_.verbose_level > 1) {
            /* For higher verbosity, we want to see the output of the test
             * unit as it is being executed. This is not possible with fork()
             * as we have to wait for the child to finish. */
            run_test_unit_(unit);
        } else {
            run_test_unit_forked_(unit);
        }
#else
        run_test_unit_(unit);
#endif
    }

    print_summary_();

end:
    output_destroy_();
    free(test_.progname);
    return (test_.failed_units == 0) ? 0 : 1;
}

#endif  /* TEST_NO_MAIN */

#endif  /* ACUTEST_IMPLEMENTATION_ */

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* ACUTEST_H__ */
