cmake_minimum_required(VERSION 3.8)
project(util)

set(CMAKE_C_STANDARD 90)

#============== util ==============

set(UTIL_HEADER_FILES
        util/headers/list.h
        util/headers/err_check.h
        util/headers/stack.h)

set(UTIL_SOURCE_FILES
        util/src/list.c
        util/src/err_check.c
        util/src/stack.c)

add_library(util ${UTIL_SOURCE_FILES} ${UTIL_HEADER_FILES})

target_include_directories(
        util PUBLIC
        util/headers
)

#============== task1 ==============

set(TASK1_SOURCE_FILES task1/src/main.c)

add_executable(task1 ${TASK1_SOURCE_FILES})

target_include_directories(
        task1 PUBLIC
        util/headers
)

target_link_libraries(task1 util)

#============== task2 ==============

set(TASK2_SOURCE_FILES task2/src/main.c)

add_executable(task2 ${TASK2_SOURCE_FILES})

target_include_directories(
        task2 PUBLIC
        util/headers
)

target_link_libraries(task2 util)

#============== task3 ==============

set(TASK3_SOURCE_FILES task3/src/main.c)

add_executable(task3 ${TASK3_SOURCE_FILES})

target_include_directories(
        task3 PUBLIC
        util/headers
)

target_link_libraries(task3 util)

#============== task4 ==============

set(TASK4_SOURCE_FILES task4/src/main.c)

add_executable(task4 ${TASK4_SOURCE_FILES})

target_include_directories(
        task4 PUBLIC
        util/headers
)

target_link_libraries(task4 util)

#============== task5 ==============

set(TASK5_SOURCE_FILES task5/src/main.c)

add_executable(task5 ${TASK5_SOURCE_FILES})

target_include_directories(
        task5 PUBLIC
        util/headers
)

target_link_libraries(task5 util)

#============== task7 ==============

set(TASK7_SOURCE_FILES task7/src/main.c)

add_executable(task7 ${TASK7_SOURCE_FILES})

target_include_directories(
        task7 PUBLIC
        util/headers
)

target_link_libraries(task7 util)
