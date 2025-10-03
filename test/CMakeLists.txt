if(BUILD_TESTING)
  add_executable(My6502LoadRegisterTests My6502LoadRegisterTests.cpp)
  target_link_libraries(My6502LoadRegisterTests PRIVATE my6502 GTest::gtest_main)
  target_include_directories(My6502LoadRegisterTests PUBLIC ../include)

  add_executable(My6502StoreRegisterTests My6502StoreRegisterTests.cpp)
  target_link_libraries(My6502StoreRegisterTests PRIVATE my6502 GTest::gtest_main)
  target_include_directories(My6502StoreRegisterTests PUBLIC ../include)

  add_executable(My6502JumpsAndCallsTests My6502JumpsAndCallsTests.cpp)
  target_link_libraries(My6502JumpsAndCallsTests PRIVATE my6502 GTest::gtest_main)
  target_include_directories(My6502JumpsAndCallsTests PUBLIC ../include)

  include(GoogleTest)
  gtest_discover_tests(My6502LoadRegisterTests DISCOVERY_MODE PRE_TEST)
  gtest_discover_tests(My6502StoreRegisterTests DISCOVERY_MODE PRE_TEST)
  gtest_discover_tests(My6502JumpsAndCallsTests DISCOVERY_MODE PRE_TEST)
endif()
