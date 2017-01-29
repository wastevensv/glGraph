cmake_minimum_required(VERSION 2.8)
project( GlGraph CXX )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package( OpenGL REQUIRED )
find_package( glfw3 REQUIRED )

add_executable(glGraph  main.cpp)
target_link_libraries(glGraph  SOIL epoxy ${OPENGL_LIBRARY} ${SOIL_LIBRARY} ${GLFW3_LIBRARY})
