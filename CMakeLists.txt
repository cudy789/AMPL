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

# Add the source files
add_executable(apriltag_localization src/multicam_opencv_demo.cpp src/LocalizationWorker.cpp src/Localization.cpp src/TagDetectorCamera.cpp src/TagDetectorCamWorker.cpp)

# Link any libraries if needed (add any libraries here)
target_link_libraries(apriltag_localization apriltag ${OpenCV_LIBS})

# Option to enable or disable tests
option(ENABLE_TESTS "Enable building tests" ON)

# Fetch GoogleTest if tests are enabled
if(ENABLE_TESTS)

    # Link google test
    include(FetchContent)
    # Fetch GoogleTest
    FetchContent_Declare(
            googletest
            # Specify the commit you depend on and update it regularly.
            URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Add the test files
    file(GLOB TEST_SOURCES "test/*.cpp")

    # Create the test executable
    add_executable(runTests ${TEST_SOURCES})

    # Link the test executable against gtest and gtest_main
    target_link_libraries(runTests gtest_main)

    # Enable testing
    enable_testing()

    # Add the tests to CTest
    add_test(NAME runTests COMMAND runTests)
endif()

# Optionally, set additional compiler flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")
