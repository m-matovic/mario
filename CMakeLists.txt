cmake_minimum_required(VERSION 3.24)
project(Mario)

set(CMAKE_CXX_STANDARD 17)
set(HEADERS MapLoader.h EntityHandler.h nuklear.h nuklear_glfw_gl3.h ui.h)

add_executable(Mario main.cpp ${HEADERS})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(Mario ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW3_LIBRARIES} ${M_LIBRARIES})
