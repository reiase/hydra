PROJECT(tests)

SET(tests
  test_resp
  test_eservice
  test_zservice
  test_parametermanager
  )

foreach(test ${tests})
  add_executable(${test} ${test}.cc)
  add_test(NAME ${test} COMMAND ${test})
  target_link_libraries(${test} pthread hydra)
endforeach()
