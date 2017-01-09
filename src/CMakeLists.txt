include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(hydra
  eservice.cc
  hydra.cc
  resp.cc
  service.cc)
target_link_libraries(hydra zmq)
