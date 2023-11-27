cmake_minimum_required(VERSION 3.10)
project(YourProjectName)

set(CMAKE_CXX_FLAGS "-O3")
find_package(OpenCV REQUIRED)
find_package(dlib REQUIRED)
find_package(LAPACK REQUIRED)
add_executable(main main.cpp cv.cpp face.cpp item.cpp)

# DlibとOpenCVのライブラリをリンク
target_link_libraries(main PRIVATE dlib ${OpenCV_LIBS} ${LAPACK_LIBRARIES} glut GL GLU)