cmake_minimum_required(VERSION 3.30.5)

project(graphics-pipeline VERSION 0.1.0 LANGUAGES CXX)

# generate compile_commands.json for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE INCLUDE_FILES src/*.hpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${INCLUDE_FILES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PRIVATE src)

if (WIN32)
	# windows C++ compilers may or may not support command line options with a -
	# so we use different options for windows
	target_compile_options(${PROJECT_NAME} PRIVATE "/Wall", "/WX")
else()
	# make sure all errors are known
	target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Werror")
	# include debug info
	target_compile_options(${PROJECT_NAME} PRIVATE "-g")
endif()

# link with sdl3, which is a library that wraps around OpenGL and provides many other utilities
find_package(SDL3 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL3_INCLUDE_DIRS})

# link with libtiff for loading and saving
find_package(TIFF REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE TIFF::TIFF)
target_include_directories(${PROJECT_NAME} PUBLIC ${TIFF_INCLUDE_DIRS})
