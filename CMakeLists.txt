cmake_minimum_required(VERSION 3.10)

# Set the project name
project(ApriltagLocalization)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Add the include directories
include_directories(include)
include_directories(lib/Eigen)
include_directories(${OpenCV_INCLUDE_DIRS})

# Add the apriltag library
add_subdirectory(lib/apriltag)

# Add the source file
add_executable(apriltag_localization src/multicam_opencv_demo.cpp src/LocalizationWorker.cpp src/Localization.cpp src/TagDetectorCamera.cpp src/TagDetectorCamWorker.cpp)

# Link any libraries if needed (add any libraries here)
target_link_libraries(apriltag_localization apriltag ${OpenCV_LIBS})


# Optionally, set additional compiler flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")
