cmake_minimum_required(VERSION 2.8)
project( GlSquare CXX )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package( OpenGL REQUIRED )
find_package( glfw3 REQUIRED )

#add_library(glhelper glhelper.h glhelper.cpp)
add_executable(glSquare main.cpp)
target_link_libraries(glSquare SOIL epoxy ${OPENGL_LIBRARY} ${SOIL_LIBRARY} ${GLFW3_LIBRARY})
