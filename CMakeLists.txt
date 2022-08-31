project(temp-check)

cmake_minimum_required(VERSION 3.12)

set(PROJ_SERVICE_NAME ${CMAKE_PROJECT_NAME}-service)

add_executable(${PROJ_SERVICE_NAME}
    src/main.cpp
)
