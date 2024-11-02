cmake_minimum_required(VERSION 3.10)
project(agent)

# Define sources
file(GLOB SOURCES "code/*.cpp")

# Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/output")

# Add jansson library as a subdirectory
add_subdirectory("jansson" "${CMAKE_BINARY_DIR}/jansson_build")

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

# Include directories
include_directories(
    ${LIBXML2_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${CURL_INCLUDE_DIR}
    ${JANSSON_INCLUDE_DIRS}
    "jdk1.8.0_91.jdk/Contents/Home/include"
    "jdk1.8.0_91.jdk/Contents/Home/include/darwin"
    "."
)

# Link directories
link_directories(${LIBXML2_LIBRARY_DIRS} ${CURL_LIBRARY_DIRS})

# Set platform-specific flags and output names
if(WIN32)
    set(LIB_NAME "agent.dll")
    add_compile_options(-DWIN32)
    add_library(${LIB_NAME} SHARED ${SOURCES})
    target_link_libraries(${LIB_NAME} ${LIBXML2_LIBRARIES} ${OPENSSL_LIBRARIES} ${CURL_LIBRARIES} jansson)
elseif(APPLE)
    set(LIB_NAME "agent.dylib")
    add_compile_options(-DAPPLE)
    add_library(${LIB_NAME} SHARED ${SOURCES})
    target_link_libraries(${LIB_NAME} ${LIBXML2_LIBRARIES} ${OPENSSL_LIBRARIES} ${CURL_LIBRARIES} jansson)
endif()

# Clean up object files
add_custom_target(clean-objects
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/*.o
    COMMENT "Cleaning up object files"
)
