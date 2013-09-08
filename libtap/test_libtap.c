
/*

$ declare -x LD_LIBRARY_PATH=/usr/local/lib

$ gcc test_libtap.c -ltap



*/

#include <stdio.h>
#include <tap.h>

int do_something_else_as_root() {
return 1;

}
int do_something_as_root() {
return 1;
}

int ultimate_answer() {

return 42;
}

int
main(void)
{
    unsigned int root = 0;

    /*
     * Plan to run a total of 4 tests.  Running fewer or more than this
     * will generate an error.
     */
    plan_tests(4);

    /*
     * A simple example of a test.
     */
    ok(1 + 1 == 2, "Basic arithmetic works");

    /* The next test(s) need to be run as root.  Rather than doing something
     * like this at the start of the program:
     *
     *  int tests_that_need_root = 4;
     *  int total_tests          = 12;
     *
     *  if(getuid() == 0)
     *    plan_tests(total_tests);
     *  else
     *    plan_tests(total_tests - tests_that_need_root);
     *
     * you can tell the test system to skip over a certain number of 
     * tests.  All the code between skip_start() and skip_end() will
     * be ignored (including any non-test code).
     */
    skip_start(getuid() != 0,   /* Boolean expression      */
               2,               /* Number of tests to skip */
               "Skipping 2 tests because we're not root");

    /* Set a flag if we're running as root.  This code will not run
       if the boolean expression in the skip_start() call evaluated 
       to false */
    root = 1;

    ok(do_something_as_root(), "Doing something as root");
    ok(do_something_else_as_root(), "Did something else as root");

     skip_end;   /* Finished skipped tests */

    ok(ultimate_answer() == 42, "The answer's correct");

    /*
     * Produce a diagnostic message that indicates whether or not the
     * code was running as root
     */
    diag("We %s run as root", root ? "did" : "did not");

    return exit_status();     /* Return the correct exit code */
}

