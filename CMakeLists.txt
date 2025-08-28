cmake_minimum_required(VERSION 3.30.5)

project(graphics-pipeline VERSION 0.1.0 LANGUAGES CXX)

# generate compile_commands.json for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE INCLUDE_FILES include/*.hpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${INCLUDE_FILES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_FILES})

# TODO: these flags won't work on windows
# make sure all errors are known
target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Werror")
# except this one, because OpenGL is deprecated on macOS
# and we get a lot of warnings about that
target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-deprecated-declarations")

# link with OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS})

# link with glfw, which is a library for handling windows for OpenGL
find_package(GLFW3 REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)
target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW3_INCLUDE_DIRS})
