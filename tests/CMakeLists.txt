PROJECT(tests)

SET(tests
  test_resp
  test_parametermanager
  test_reactor
  )

foreach(test ${tests})
  add_executable(${test} ${test}.cc)
  add_test(NAME ${test} COMMAND ${test})
  target_link_libraries(${test} pthread hydra)
endforeach()
