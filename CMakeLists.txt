cmake_minimum_required(VERSION 2.8)
project(main)

find_package(OpenCV REQUIRED)
add_executable(main main.cpp Camera.cpp YmlRW.cpp)
target_link_libraries(main ${OpenCV_LIBS})