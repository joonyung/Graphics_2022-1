cmake_minimum_required(VERSION 3.10)
cmake_policy(VERSION 3.10)
project("Lab1" C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

set(PROJECT_ORIGIN "../../")

file(GLOB SOURCE
    "src/*.h"
    "src/*.cpp"
)
set(SOURCE ${SOURCE} ../glad.c)
set(NAME "main")

link_directories(${PROJECT_ORIGIN}/lib)

if(WIN32)
  set(LIBS glfw3 opengl32)
  list(APPEND CMAKE_CXX_FLAGS "-std=c++11")
endif(WIN32)

if(UNIX)
  find_package(PkgConfig REQUIRED)
  # pkg_search_module(GLFW REQUIRED glfw3)
  # find_package(glm REQUIRED)
  # set(LIBS ${GLFW_STATIC_LIBRARIES})
  # include_directories(${GLFW_INCLUDE_DIRS})
  # include_directories(${GLM_INCLUDE_DIRS})

  INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
  FIND_LIBRARY(IOKit_LIBRARY IOKit)
  FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
  MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
  SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
  SET(APPLE_LIBS ${APPLE_LIBS} ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} ${FREETYPE_LIBRARIES})
  set(LIBS ${LIBS} ${APPLE_LIBS})
endif(UNIX)

add_executable(${NAME} ${SOURCE})
target_link_libraries(${NAME} ${LIBS})
include_directories(${PROJECT_ORIGIN}/includes)