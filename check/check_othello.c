#include <stdlib.h>
#include <check.h>

#include <othello.h>

// void setup(void) { 
// }
// 
// void teardown(void) {
// }

START_TEST (test_create) {
    int rc = 0;
    t_board board;
    rc = initBoard(&board);
    fail_unless(rc, "Initialization returned false.");
    fail_unless(board.turn == BLACK,
                "Turn not set correctly: Black begins.");
    fail_unless(board.moveCount == 4,
                "Board should begin with 4 possible moves. was: %d", board.moveCount);
    fail_unless(board.move == 1,
                "After initialization, this should be the first move. It is: %d now",
                board.move);
}
END_TEST


Suite *othello_suite(void) {
    Suite *s = suite_create("Othello");
    TCase *tc_core = tcase_create("Core");
    // TCase *tc_other = tcase_create("Other");

    suite_add_tcase(s, tc_core);
    // suite_add_tcase(s, tc_other);

    tcase_add_test(tc_core, test_create);
    // tcase_add_checked_fixture(tc_core, setup, teardown);
    
    // tcase_add_test(tc_other, test_other);

    return s;
}

int main(void) {
  int nf;
  Suite *s = othello_suite();
  SRunner *sr = srunner_create(s);
  srunner_set_log(sr, "check-othello.log");
  srunner_run_all(sr, CK_NORMAL);
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
