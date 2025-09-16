cmake_minimum_required(VERSION 3.30.5)

project(graphics-pipeline VERSION 0.1.0 LANGUAGES CXX)

# generate compile_commands.json for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE INCLUDE_FILES src/*.hpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${INCLUDE_FILES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PRIVATE src)

# windows C++ compilers require different command line options from Linux/MacOS
if (WIN32)
	# make sure all errors are known
	target_compile_options(${PROJECT_NAME} PRIVATE "/W4" "/WX")
	# include debug info
	target_compile_options(${PROJECT_NAME} PRIVATE "/Zi" "/DEBUG")

	# load libraries for windows manually
	target_link_libraries(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/external/SDL3/lib/SDL3.lib")
	target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/external/SDL3/include")
	
	# tiff for image loading/saving
	target_link_libraries(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/external/tiff-4.0.8/libtiff/libtiff.lib")
	target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${PROJECT_SOURCE_DIR}/external/tiff-4.0.8/libtiff")

	# copy dlls to the build directory
	add_custom_command (
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy -t "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
                "${PROJECT_SOURCE_DIR}/external/SDL3/bin/SDL3.dll"
				"${PROJECT_SOURCE_DIR}/external/tiff-4.0.8/libtiff/libtiff.dll"
				USES_TERMINAL COMMAND_EXPAND_LISTS
    )
else()
	# make sure all errors are known
	target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Werror")
	# include debug info
	target_compile_options(${PROJECT_NAME} PRIVATE "-g")
	
	
	# load libraries for linux/macos
	find_package(SDL3 REQUIRED)
	find_package(TIFF REQUIRED)
	
	# link with sdl3, which is a library that wraps around OpenGL and provides many other utilities
	target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
	target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC SDL3::Headers)

	# link with libtiff for loading and saving
	target_link_libraries(${PROJECT_NAME} PRIVATE TIFF::TIFF)
	target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${TIFF_INCLUDE_DIRS})
endif()

# copy geometry directory to build directory
file(COPY "${PROJECT_SOURCE_DIR}/geometry" DESTINATION ".")
