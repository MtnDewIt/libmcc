cmake_minimum_required(VERSION 3.14)
project(libmcc)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
    detours
    GIT_REPOSITORY https://github.com/microsoft/Detours.git
    GIT_TAG v4.0.1
)

FetchContent_MakeAvailable(detours)

if(EXISTS "${detours_SOURCE_DIR}/include/detours.h")
    set(DETOURS_INCLUDE_DIR "${detours_SOURCE_DIR}/include")
elseif(EXISTS "${detours_SOURCE_DIR}/src/detours.h")
    set(DETOURS_INCLUDE_DIR "${detours_SOURCE_DIR}/src")
else()
    message(FATAL_ERROR "Could not find detours.h under ${detours_SOURCE_DIR}")
endif()

set(DETOURS_SRC_DIR "${detours_SOURCE_DIR}/src")

add_library(detours_static STATIC
    "${DETOURS_SRC_DIR}/creatwth.cpp"
    "${DETOURS_SRC_DIR}/detours.cpp"
    "${DETOURS_SRC_DIR}/disasm.cpp"
    "${DETOURS_SRC_DIR}/disolx64.cpp"
    "${DETOURS_SRC_DIR}/image.cpp"
    "${DETOURS_SRC_DIR}/modules.cpp"
)

target_include_directories(detours_static PUBLIC "${DETOURS_INCLUDE_DIR}")

add_library(libmcc STATIC
    ./src/libmcc.cpp
    ./src/memory/module.cpp
)

target_include_directories(libmcc PUBLIC ./include)
target_include_directories(libmcc PRIVATE "${DETOURS_INCLUDE_DIR}")
target_link_libraries(libmcc PRIVATE detours_static)
