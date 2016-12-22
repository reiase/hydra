include(ExternalProject)

SET(CMAKE_SKIP_RPATH false)

ExternalProject_Add(zmq3rd
  GIT_REPOSITORY "http://github.com/zeromq/libzmq.git"
  GIT_TAG "master"

  UPDATE_COMMAND ""
  PATCH_COMMAND ""

  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/src/zmq
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/

  INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/
  )
