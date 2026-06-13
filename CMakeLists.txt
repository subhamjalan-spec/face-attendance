cmake_minimum_required(VERSION 3.15)
project(FaceAttendance VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV (Homebrew path on Mac)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV libs: ${OpenCV_LIBS}")

# Source files
set(SOURCES
    src/main.cpp
    src/trainer.cpp
    src/recognizer.cpp
    src/logger.cpp
)

add_executable(face_attendance ${SOURCES})
target_link_libraries(face_attendance ${OpenCV_LIBS})

# Copy cascade file to build directory for easy access
configure_file(
    ${CMAKE_SOURCE_DIR}/data/haarcascade_frontalface_default.xml
    ${CMAKE_BINARY_DIR}/haarcascade_frontalface_default.xml
    COPYONLY
)