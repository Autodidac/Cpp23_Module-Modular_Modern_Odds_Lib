cmake_minimum_required(VERSION 3.28)

project(ayejay_odds
    VERSION 1.0.0
    LANGUAGES CXX
)

include(cmake/EpochOptions.cmake)

# ---- Library ----
add_library(ayejay_odds STATIC)

target_sources(ayejay_odds
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        FILES
            include/ayejay/odds.hpp
)

if(EPOCH_ENABLE_MODULES)
    target_sources(ayejay_odds
        PUBLIC
            FILE_SET CXX_MODULES
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/modules
            FILES
                modules/ayejay.odds.cppm
    )
endif()

target_compile_features(ayejay_odds PUBLIC cxx_std_23)

target_include_directories(ayejay_odds
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_options(ayejay_odds
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /Zc:preprocessor /EHsc>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

# ---- Example executable ----
add_executable(ayejay_odds_example
    examples/main.cpp
)

target_link_libraries(ayejay_odds_example
    PRIVATE ayejay_odds
)

set_target_properties(ayejay_odds PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

set_target_properties(ayejay_odds_example PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
