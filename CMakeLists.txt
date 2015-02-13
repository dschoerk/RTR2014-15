include(ExternalProject)
enable_testing()

cmake_minimum_required(VERSION 2.8)
project(Candy)

set(CANDY_MAJOR_VERSION 0)
set(CANDY_MINOR_VERSION 0)
set(CANDY_PATCH_VERSION 0)
set(CANDY_VERSION
  ${CANDY_MAJOR_VERSION}.${CANDY_MINOR_VERSION}.${CANDY_PATCH_VERSION})

ExternalProject_Add(
	Glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	CMAKE_CACHE_ARGS "-DBUILD_SHARED_LIBS:BOOL=OFF"
    UPDATE_COMMAND ""
	INSTALL_COMMAND ""
)

link_directories("${CMAKE_BINARY_DIR}/Glfw-prefix/src/Glfw-build/src/Release") # Currently always use debug build/src/Debug
include_directories("${CMAKE_BINARY_DIR}/Glfw-prefix/src/Glfw/include")

ExternalProject_Add(
	Assimp
	GIT_REPOSITORY https://github.com/assimp/assimp.git
	CMAKE_CACHE_ARGS "-DBUILD_SHARED_LIBS:BOOL=OFF"
    UPDATE_COMMAND ""
	INSTALL_COMMAND ""
)


link_directories("${CMAKE_BINARY_DIR}/Assimp-prefix/src/Assimp-build/code/Debug")
link_directories("${CMAKE_BINARY_DIR}/Assimp-prefix/src/Assimp-build/code/Release")
include_directories("${CMAKE_BINARY_DIR}/Assimp-prefix/src/Assimp/include")

ExternalProject_Add(
	Glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
    UPDATE_COMMAND ""
	INSTALL_COMMAND ""
)
include_directories("${CMAKE_BINARY_DIR}/Glm-prefix/src/Glm")

ExternalProject_Add(
	Gli
	GIT_REPOSITORY https://github.com/g-truc/gli.git
    UPDATE_COMMAND ""
	INSTALL_COMMAND ""
)
include_directories("${CMAKE_BINARY_DIR}/Gli-prefix/src/Gli")


ExternalProject_Add(
	Glew
	URL http://sourceforge.net/projects/glew/files/glew/1.11.0/glew-1.11.0-win32.zip
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
    BUILD_IN_SOURCE 1
#    UPDATE_COMMAND ""
	INSTALL_COMMAND ""
	LOG_DOWNLOAD 1
	LOG_UPDATE 1
	LOG_CONFIGURE 1
	LOG_BUILD 1
	LOG_TEST 1
	LOG_INSTALL 1
)


link_directories("${CMAKE_SOURCE_DIR}/ext/FreeImage/vc120_x64")
include_directories("${CMAKE_SOURCE_DIR}/ext/FreeImage/vc120_x64")

link_directories("${CMAKE_SOURCE_DIR}/ext/zlib")
#include_directories("${CMAKE_SOURCE_DIR}/ext/irrKlang")

link_directories("${CMAKE_SOURCE_DIR}/ext/irrKlang")
include_directories("${CMAKE_SOURCE_DIR}/ext/irrKlang")

include_directories(
  "${PROJECT_SOURCE_DIR}/inc"   # to find foo/foo.h
  "${PROJECT_SOURCE_DIR}/inc/GL"
  "${PROJECT_BINARY_DIR}")  # to find foo/config.h

FILE(GLOB SRC "src/*.cpp")
FILE(GLOB SRC_GL "src/GL/*.cpp")
FILE(GLOB SRC_SCENE "src/Scene/*.cpp")
FILE(GLOB INC "inc/*.h")
FILE(GLOB INC_SCENE "inc/Scene/*.h")
FILE(GLOB INC_GL "inc/GL/*.h")

message(${SRC})

	link_directories("${CMAKE_BINARY_DIR}/Glew-prefix/src/Glew/lib/Release/x64")
	include_directories("${CMAKE_BINARY_DIR}/Glew-prefix/src/Glew/include")

add_executable(Candy ${SRC} ${INC} ${SRC_GL} ${INC_GL} ${SRC_SCENE} ${INC_SCENE} )

#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(Candy glfw3 glew32 opengl32 assimp freeimage-x64 irrKlang zlibstatic)
#else()
#	target_link_libraries(Candy glfw3 glew32 opengl32 assimp freeimage-x64 irrKlang zlibstatic)
#endif()





# https://github.com/glfw/glfw.git
