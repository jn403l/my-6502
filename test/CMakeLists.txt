if(BUILD_TESTING)
  add_executable(test_test test_test.cpp)
  target_link_libraries(test_test PRIVATE my6502 GTest::gtest_main)
  target_include_directories(test_test PUBLIC ../include)

  include(GoogleTest)
  gtest_discover_tests(test_test DISCOVERY_MODE PRE_TEST)
endif()
