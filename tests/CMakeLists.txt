find_library(REQUIRED GTest)

find_package(GTest REQUIRED)

file(GLOB TESTS *.cpp)

add_executable(immu_tests ${TESTS})

target_link_libraries(immu_tests
    immu
    GTest::GTest
    GTest::Main
)

gtest_add_tests(TARGET immu_tests)
