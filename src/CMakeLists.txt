include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(hydra
  hydra.cc
  service.cc)
target_link_libraries(hydra)
