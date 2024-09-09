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
add_executable(apriltag_localization src/LocalizationWorker.cpp src/Localization.cpp src/TDCam.cpp src/TDCamWorker.cpp src/main.cpp)

# Yaml parser
include(FetchContent)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG 0.8.0 # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(yaml-cpp)

# Json parser

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# Link any libraries if needed (add any libraries here)
target_link_libraries(apriltag_localization PUBLIC yaml-cpp::yaml-cpp apriltag ${OpenCV_LIBS} PRIVATE nlohmann_json::nlohmann_json)

# Option to enable or disable tests
option(ENABLE_TESTS "Enable building tests" OFF)

# Fetch GoogleTest if tests are enabled
if(ENABLE_TESTS)

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
    add_executable(runTests ${TEST_SOURCES} src/LocalizationWorker.cpp src/Localization.cpp src/TDCam.cpp src/TDCamWorker.cpp)

    # Link the test executable against gtest and gtest_main
    target_link_libraries(runTests PUBLIC yaml-cpp::yaml-cpp gtest_main  apriltag ${OpenCV_LIBS} PRIVATE nlohmann_json::nlohmann_json)

    # Enable testing
    enable_testing()

    # Add the tests to CTest
    add_test(NAME runTests COMMAND runTests)
endif()

# Optionally, set additional compiler flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")
