CMAKE_MINIMUM_REQUIRED (VERSION 2.8.4)
PROJECT(greenskin)

set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/;${CMAKE_MODULE_PATH}")
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/dist)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(DEFAULT_DEFINES 
  ${DEFAULT_DEFINES}
  "-DGS_ENABLE_ASSERTS"
  "-DGS_ENABLE_TRACES"
  "-DGS_DEBUG")

IF(UNIX)
  set(DEFAULT_DEFINES 
    ${DEFAULT_DEFINES}
    "-DGS_OS_LINUX"
    "-DGS_COMPILER_GCC")
ENDIF(UNIX)

IF( WIN32 )
ENDIF(WIN32)

add_definitions(${DEFAULT_DEFINES})
add_subdirectory(libs/gtest)                                                    
set(GTEST_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/libs/gtest/include" )               
enable_testing()                                                                
set(GTEST_LIBRARIES "gtest") 

add_subdirectory(src)
