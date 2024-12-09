cmake_minimum_required(VERSION 3.10)

# Set the project name
project(ApriltagMulticameraPoseLocalization)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the default build type to Release if none is provided
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()
# Specify the possible build types
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

include_directories(include)
file(GLOB AMPL_SOURCES "src/*.cpp")
list(REMOVE_ITEM AMPL_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")
add_executable(ampl ${AMPL_SOURCES} "src/main.cpp")

# Find the NTCore package within allwpilib
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
add_subdirectory(lib/allwpilib)

# Find OpenCV package
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Find Eigen
include_directories(lib/Eigen)

# Find Mongoose
add_library(mongoose STATIC lib/mongoose/mongoose.c)
target_include_directories(mongoose PUBLIC lib/mongoose)

configure_file(src/web/index.html ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(src/web/js/three.module.js ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(src/web/js/three.core.js ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(src/web/js/OrbitControls.js ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

# Add the apriltag library, do not build examples or python wrapper
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_PYTHON_WRAPPER OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/apriltag)

file(MAKE_DIRECTORY "logs")

# Yaml parser
add_subdirectory(lib/yaml-cpp)

# Json parser
add_subdirectory(lib/json)

target_link_libraries(ampl PUBLIC yaml-cpp::yaml-cpp ${OpenCV_LIBS} ntcore apriltag PRIVATE nlohmann_json::nlohmann_json mongoose)

# Option to enable or disable tests
option(ENABLE_TESTS "Enable building tests" OFF)

# Fetch GoogleTest if tests are enabled
if(ENABLE_TESTS)

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
    add_executable(runTests ${TEST_SOURCES} ${AMPL_SOURCES})

    # Link the test executable against gtest and gtest_main
    target_link_libraries(runTests PUBLIC yaml-cpp::yaml-cpp gtest_main ${OpenCV_LIBS} ntcore PRIVATE nlohmann_json::nlohmann_json apriltag mongoose)

    # Enable testing
    enable_testing()

    # Add the tests to CTest
    add_test(NAME runTests COMMAND runTests)
endif()