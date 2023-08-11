# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("uranus")

include_directories(include)
include_directories(soundtouch/include)
include_directories(soundtouch/SoundTouch)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}")

aux_source_directory(. SOURCE)
aux_source_directory(./soundtouch/SoundTouch SOUND_TOUCH_SOURCE)

add_library(${CMAKE_PROJECT_NAME}
        SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
        ${SOURCE}
        ${SOUND_TOUCH_SOURCE})


target_link_libraries(${CMAKE_PROJECT_NAME}
        # List libraries link to the target library
        avcodec
        avfilter
        avformat
        avutil
        swresample
        swscale
        OpenSLES
        android
        log)