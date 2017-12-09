cmake_minimum_required(VERSION 3.8)
project(util)

set(CMAKE_C_STANDARD 90)

#============== util ==============

set(UTIL_HEADER_FILES
        util/include/list.h
        util/include/err_check.h
        util/include/stack.h util/include/parse.h util/include/usage.h util/include/bubble_sort.h)

set(UTIL_SOURCE_FILES
        util/src/list.c
        util/src/err_check.c
        util/src/stack.c util/include/parse.h util/src/parse.c util/include/usage.h util/src/usage.c util/include/bubble_sort.h util/src/bubble_sort.c)

add_library(util ${UTIL_SOURCE_FILES} ${UTIL_HEADER_FILES})

target_include_directories(
        util PUBLIC
        util/include
)

#============== task1 ==============

set(TASK1_SOURCE_FILES task1/src/main.c)

add_executable(task1 ${TASK1_SOURCE_FILES})

target_include_directories(
        task1 PUBLIC
        util/include
)

target_link_libraries(task1 util)

#============== task2 ==============

set(TASK2_SOURCE_FILES task2/src/main.c)

add_executable(task2 ${TASK2_SOURCE_FILES})

target_include_directories(
        task2 PUBLIC
        util/include
)

target_link_libraries(task2 util)

#============== task3 ==============

set(TASK3_SOURCE_FILES task3/src/main.c)

add_executable(task3 ${TASK3_SOURCE_FILES})

target_include_directories(
        task3 PUBLIC
        util/include
)

target_link_libraries(task3 util)

#============== task4 ==============

set(TASK4_SOURCE_FILES task4/src/main.c)

add_executable(task4 ${TASK4_SOURCE_FILES})

target_include_directories(
        task4 PUBLIC
        util/include
)

target_link_libraries(task4 util)

#============== task5 ==============

set(TASK5_SOURCE_FILES task5/src/main.c)

add_executable(task5 ${TASK5_SOURCE_FILES})

target_include_directories(
        task5 PUBLIC
        util/include
)

target_link_libraries(task5 util)

#============== task7 ==============

set(TASK7_SOURCE_FILES task7/src/main.c task7/include/helpers.h task7/src/helpers.c)

add_executable(task7 ${TASK7_SOURCE_FILES})

target_include_directories(
        task7 PUBLIC
        task7/include
        util/include
)

target_link_libraries(task7 util)

#============== task8 ==============

set(TASK8_SOURCE_FILES task8/src/main.c task8/src/helpers.c task8/include/helpers.h)

add_executable(task8 ${TASK8_SOURCE_FILES})

target_include_directories(
        task8 PUBLIC
        task8/include
        util/include
)

target_link_libraries(task8 util)

#============== task9 ==============

set(TASK9_SOURCE_FILES task9/src/main.c task9/src/dinner.c task9/include/dinner.h)

add_executable(task9 ${TASK9_SOURCE_FILES})

target_include_directories(
        task9 PUBLIC
        task9/include
        util/include
)

target_link_libraries(task9 util)

#============== task10 ==============

set(TASK10_SOURCE_FILES task10/src/main.c)

add_executable(task10 ${TASK10_SOURCE_FILES})

target_include_directories(
        task10 PUBLIC
        util/include
)

target_link_libraries(task10 util)

#============== task12 ==============

set(TASK12_SOURCE_FILES task12/src/main.c)

add_executable(task12 ${TASK12_SOURCE_FILES})

target_include_directories(
        task12 PUBLIC
        util/include
)

target_link_libraries(task12 util)

#============== task13 ==============

set(TASK13_SOURCE_FILES task13/src/main.c)

add_executable(task13 ${TASK13_SOURCE_FILES})

target_include_directories(
        task13 PUBLIC
        util/include
)

target_link_libraries(task12 util)