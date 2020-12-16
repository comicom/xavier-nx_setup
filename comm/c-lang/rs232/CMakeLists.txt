## cmake 최소 필요 버전
cmake_minimum_required (VERSION 2.8)
 
## 프로젝트명
project (UARTcomm)

## 
set(CMAKE_BUILD_TYPE Release)
 
##
include_directories(${PROJET_SOURCE_DIR})

## -D 로 전달할 pre defined macro 가 있을 경우 사용
add_definitions(-D__MY_DEFINED__)

# Recurse into the "Hello" and "Demo" subdirectories. This does not actually cause another cmake executable to run. The same process will walk through the project's entire directory structure.
add_executable(app demo_main.c rs232.c)