CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)

PROJECT(hydra)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#add_subdirectory(3rdparty)

#INCLUDE_DIRECTORIES(
#  include
#  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/include)
#
#LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/3rdparty/lib)

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
