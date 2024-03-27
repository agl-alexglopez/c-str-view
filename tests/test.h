#ifndef TEST
#define TEST

enum test_result
{
    ERROR = -1,
    PASS = 0,
    FAIL = 1,
};

typedef enum test_result (*test_fn)(void);

struct fn_name
{
    test_fn fn;
    const char *const name;
};

/* Set this breakpoint on any line where you wish
   execution to stop. Under normal program runs the program
   will simply exit. If triggered in GDB execution will stop
   while able to explore the surrounding context, varialbes,
   and stack frames. Be sure to step "(gdb) up" out of the
   raise function to wherever it triggered. */
#define BREAKPOINT()                                                           \
    do                                                                         \
    {                                                                          \
        (void)fprintf(stderr, "\n!!Break. Line: %d File: %s, Func: %s\n ",     \
                      __LINE__, __FILE__, __func__);                           \
        (void)raise(SIGTRAP);                                                  \
    } while (0)

/* The CHECK macro evaluates a result against an expecation as one
   may be familiar with in many testing frameworks. However, it is
   expected to execute in a function where a test_result is returned.
   Provide the resulting operation against the expected outcome. The
   types must be comparable with ==/!=. Finally, provide the format
   specifier for the types being compared which also must be the same
   for both RESULT and EXPECTED (e.g. "%d", "%zu", "%b"). Note that
   if either RESULT or EXPECTED are function calls, they must not
   have side effects because they may be called more than once. */
#define CHECK(RESULT, EXPECTED, TYPE_FORMAT_SPECIFIER)                         \
    do                                                                         \
    {                                                                          \
        if ((RESULT) != (EXPECTED))                                            \
        {                                                                      \
            (void)fprintf(stderr,                                              \
                          "the following check failed on line %d:\nEXPECTED: " \
                          "RESULT( %s ) == EXPECTED( %s )\n",                  \
                          __LINE__, #RESULT, #EXPECTED);                       \
            (void)fprintf(stderr, "ACTUALLY: RESULT( ");                       \
            (void)fprintf(stderr, TYPE_FORMAT_SPECIFIER, RESULT);              \
            (void)fprintf(stderr, " ) != EXPECTED( ");                         \
            (void)fprintf(stderr, TYPE_FORMAT_SPECIFIER, EXPECTED);            \
            (void)fprintf(stderr, " )\n");                                     \
            return FAIL;                                                       \
        }                                                                      \
    } while (0)

#endif
