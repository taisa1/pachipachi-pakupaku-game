cmake_minimum_required(VERSION 3.10)
project(YourProjectName)

find_package(OpenCV REQUIRED)
find_package(dlib REQUIRED)
add_executable(mouth mouth.cpp)

# DlibとOpenCVのライブラリをリンク
target_link_libraries(mouth PRIVATE dlib ${OpenCV_LIBS})
# libblasをリンク
# LAPACKとBLASをリンク
find_package(LAPACK REQUIRED)
if(LAPACK_FOUND)
    target_link_libraries(mouth PRIVATE ${LAPACK_LIBRARIES})
else()
    message(FATAL_ERROR "LAPACK not found")
endif()