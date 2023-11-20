cmake_minimum_required(VERSION 3.10)
project(YourProjectName)

set(CMAKE_CXX_FLAGS "-O3")
find_package(OpenCV REQUIRED)
find_package(dlib REQUIRED)
add_executable(main main.cpp cv.cpp face.cpp item.cpp)

# DlibとOpenCVのライブラリをリンク
target_link_libraries(main PRIVATE dlib ${OpenCV_LIBS} glut GL GLU)
# libblasをリンク
# LAPACKとBLASをリンク
find_package(LAPACK REQUIRED)
if(LAPACK_FOUND)
    target_link_libraries(main PRIVATE ${LAPACK_LIBRARIES})
else()
    message(FATAL_ERROR "LAPACK not found")
endif()