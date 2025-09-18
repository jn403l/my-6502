if(BUILD_TESTING)
  add_executable(test_test test_test.cpp)
  target_link_libraries(test_test PRIVATE GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(test_test)
endif()
