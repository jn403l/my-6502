add_test([=[My6502Test1.RunALittleInlineProgram]=]  /home/jn403l/Documents/my-6502/build/my-6502-test/test_test [==[--gtest_filter=My6502Test1.RunALittleInlineProgram]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[My6502Test1.RunALittleInlineProgram]=]  PROPERTIES WORKING_DIRECTORY /home/jn403l/Documents/my-6502/build/my-6502-test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  test_test_TESTS My6502Test1.RunALittleInlineProgram)
