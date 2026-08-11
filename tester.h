#pragma once

typedef int (*tester_utest_fn)(char* test_name);

int tester_utest_execute(char* test_name, tester_utest_fn utest_fn);
void unit_test_printer(int status, char* test_name, char* fail_dettails);



//void tester_itest();
//void tester_itest_header();
//void tester_itest_footer();