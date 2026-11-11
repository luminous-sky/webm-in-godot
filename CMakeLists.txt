cmake_minimum_required(VERSION 3.18)
project(GodotWebm)

option(DEBUG "Adds debug flags" OFF)

add_executable(out)
target_sources(out
    PRIVATE
        src/main.cpp
)
target_include_directories(out
    PRIVATE thirdparty/include
)
target_link_directories(out
    PRIVATE thirdparty/lib
)

target_compile_options(out
    PRIVATE -Wall
)
if(DEBUG)
    target_compile_options(out
        PRIVATE -g -fsanitize=address
    )
    target_link_options(out
        PRIVATE -fsantiize=address
    )
endif()
