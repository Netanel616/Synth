cmake_minimum_required(VERSION 4.1)
project(Synth1 C)

set(CMAKE_C_STANDARD 11)

set(SDL2_PATH "/opt/homebrew/opt/sdl2")
set(SDL2_TTF_PATH "/opt/homebrew/opt/sdl2_ttf")
set(SDL2_MIXER_PATH "/opt/homebrew/opt/sdl2_mixer")

# Add the 'include' directory to the include path
include_directories(include)

# Main Application
add_executable(Synth1
        src/main.c
        src/input.c
        src/setup.c
        src/gui.c
        src/audio.c
        src/effects.c
)

target_include_directories(Synth1 PRIVATE
        ${SDL2_PATH}/include/SDL2
        ${SDL2_TTF_PATH}/include/SDL2
        ${SDL2_MIXER_PATH}/include/SDL2
)

target_link_libraries(Synth1 PRIVATE
        "${SDL2_PATH}/lib/libSDL2.dylib"
        "${SDL2_TTF_PATH}/lib/libSDL2_ttf.dylib"
        "-framework Cocoa"
)

# Unit Tests
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    add_executable(run_tests tests/test_effects.c)

    # Tests need access to project headers
    target_include_directories(run_tests PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${SDL2_PATH}/include/SDL2
            ${SDL2_TTF_PATH}/include/SDL2
    )

    # Tests need to link against SDL because common.h uses SDL types
    target_link_libraries(run_tests PRIVATE
            "${SDL2_PATH}/lib/libSDL2.dylib"
            "${SDL2_TTF_PATH}/lib/libSDL2_ttf.dylib"
            "-framework Cocoa"
    )
endif()