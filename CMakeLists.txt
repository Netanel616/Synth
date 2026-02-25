cmake_minimum_required(VERSION 4.1)
project(Synth1 C)

set(CMAKE_C_STANDARD 11)

set(SDL2_PATH "/opt/homebrew/opt/sdl2")
set(SDL2_TTF_PATH "/opt/homebrew/opt/sdl2_ttf")
set(SDL2_MIXER_PATH "/opt/homebrew/opt/sdl2_mixer")

add_executable(Synth1 main.c
        common.h
        input.h
        gui.h
        audio.h
        input.c
        setup.h
        setup.c
        gui.c
        audio.c
)
# איפה לחפש קבצי .h
target_include_directories(Synth1 PRIVATE
        ${SDL2_PATH}/include/SDL2
        ${SDL2_TTF_PATH}/include/SDL2
        ${SDL2_MIXER_PATH}/include/SDL2
)

# איפה לחפש את קבצי הספריה (.dylib ב-Mac)
target_link_libraries(Synth1 PRIVATE
        "${SDL2_PATH}/lib/libSDL2.dylib"
        "${SDL2_TTF_PATH}/lib/libSDL2_ttf.dylib"
        #"${SDL2_MIXER_PATH}/lib/libSDL2_mixer.dylib"
        "-framework Cocoa" # נדרש ב-Mac
)