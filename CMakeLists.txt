cmake_minimum_required(VERSION 3.1)

cmake_policy(SET CMP0057 NEW)

project(Immu)

enable_testing()

set(CMAKE_CXX_STANDARD 11)

file(GLOB INCLUDES include/immu/*.h)

add_library(immu INTERFACE)

target_sources(immu INTERFACE ${INCLUDES})

target_include_directories(immu INTERFACE include)

add_subdirectory(tests)
