cmake_minimum_required(VERSION 3.10)

# Set the project name
project(ApriltagLocalization)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the default build type to Release if none is provided
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()
# Specify the possible build types
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Find boost for beast webserver
find_package(Boost REQUIRED COMPONENTS system)

# Add the include directories
include_directories(include)
include_directories(lib/Eigen)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIR})

# Add the source files
add_executable(apriltag_localization src/LocalizationWorker.cpp src/Localization.cpp src/TDCam.cpp src/TDCamWorker.cpp src/WebServerWorker.cpp src/NTWorker.cpp src/main.cpp)

# Add the apriltag library, do not build examples or python wrapper
add_subdirectory(lib/apriltag)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_PYTHON_WRAPPER OFF CACHE BOOL "" FORCE)

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

# NetworkTables library
FetchContent_Declare(
        ntcore
        GIT_REPOSITORY https://github.com/wpilibsuite/allwpilib.git
        GIT_TAG        main # or specify a stable release tag
)
# Set options to disable unnecessary components of allwpilib in NetworkTables
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(WITH_JAVA OFF CACHE BOOL "" FORCE)
set(WITH_JAVA_SOURCE OFF CACHE BOOL "" FORCE)
set(WITH_DOCS OFF CACHE BOOL "" FORCE)
set(WITH_CSCORE OFF CACHE BOOL "" FORCE)
set(WITH_NTCORE ON CACHE BOOL "" FORCE)  # Make sure ntcore is enabled
set(WITH_WPIMATH OFF CACHE BOOL "" FORCE)
set(WITH_WPIUNITS OFF CACHE BOOL "" FORCE)
set(WITH_WPILIB OFF CACHE BOOL "" FORCE)
set(WITH_EXAMPLES OFF CACHE BOOL "" FORCE)
set(WITH_TESTS OFF CACHE BOOL "" FORCE)
set(WITH_GUI OFF CACHE BOOL "" FORCE)
set(WITH_SIMULATION_MODULES OFF CACHE BOOL "" FORCE)
set(WITH_PROTOBUF OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(ntcore)

# Link any libraries if needed (add any libraries here)
#target_link_libraries(apriltag_localization PUBLIC yaml-cpp::yaml-cpp apriltag ${OpenCV_LIBS} ${Boost_LIBRARIES} ntcore PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(apriltag_localization PUBLIC yaml-cpp::yaml-cpp ${OpenCV_LIBS} ${Boost_LIBRARIES} ntcore apriltag PRIVATE nlohmann_json::nlohmann_json)

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
    add_executable(runTests ${TEST_SOURCES} src/LocalizationWorker.cpp src/Localization.cpp src/TDCam.cpp src/WebServerWorker.cpp src/TDCamWorker.cpp src/NTWorker.cpp)

    # Link the test executable against gtest and gtest_main
    target_link_libraries(runTests PUBLIC yaml-cpp::yaml-cpp gtest_main ${OpenCV_LIBS} ntcore PRIVATE nlohmann_json::nlohmann_json apriltag)

    # Enable testing
    enable_testing()

    # Add the tests to CTest
    add_test(NAME runTests COMMAND runTests)
endif()

# Optionally, set additional compiler flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")
