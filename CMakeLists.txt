cmake_minimum_required(VERSION 3.19)
project(3DProject)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -lpthread -lGL -lGLU -lglfw -lGLEW -lassimp")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g -lpthread -lGL -lGLU -lglfw -lGLEW -lassimp")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -g -lpthread -lGL -lGLU -lglfw -lGLEW -lassimp")



INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}/src/misc/ImGui
)
link_directories(
        ${PROJECT_SOURCE_DIR}/src/misc/ImGui
)

FILE(GLOB_RECURSE src_SRCS src/*.cpp)
FILE(GLOB_RECURSE src_HDRS src/*.h)


ADD_EXECUTABLE(3DProject
        ${src_SRCS} ${src_HDRS}
        )

target_include_directories(3DProject PUBLIC
        ${PROJECT_SOURCE_DIR}/lib
        ${PROJECT_SOURCE_DIR}/src
        ${ASSIMP_LIBRARIES}
        ${GLEW_LIBRARIES}
        GLEW::GLEW
        )