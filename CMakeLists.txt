project(temp-check)

cmake_minimum_required(VERSION 3.12)

set(PROJ_SERVICE_NAME ${CMAKE_PROJECT_NAME}-service)

add_executable(${PROJ_SERVICE_NAME}
    src/main.cpp
)

add_executable(tcc
    src/temp-check-client.cpp
)

find_package(Threads REQUIRED)

target_link_libraries(${PROJ_SERVICE_NAME}
    Threads::Threads
)

target_link_libraries(tcc
    Threads::Threads
)
