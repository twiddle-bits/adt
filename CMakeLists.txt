# CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(adt)

set(ver_maj 0)
set(ver_min 1)
set(ver_patch 0)
set(ver_string ${ver_maj}.${ver_min}.${ver_patch})

macro(msg m)
  message("[adt] ${m}")
endmacro(msg)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(PROJECT_VERSION ${ver_string})

if (UNIX)
  if(APPLE)
    set(CMAKE_INSTALL_NAME_DIR "@rpath/")
  else()
    set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
  endif()
endif(UNIX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(3rdparty.cmake)

msg("Got project source dir: ${PROJECT_SOURCE_DIR}")

file(GLOB_RECURSE export_hdr ${PROJECT_SOURCE_DIR}/export/*.h ${PROJECT_SOURCE_DIR}/export/*.hpp)

file(GLOB_RECURSE src_files ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/src/*.cpp)

msg("Got export hdrs: ${export_hdr}")
msg("Got src files: ${src_files}")

add_library(adt ${src_files})

if (BUILD_SHARED_LIBS)
  set(build_flag BUILDING_ADT)
else()
  set(build_flag )
endif()

target_compile_definitions(
  adt
  PRIVATE
    ${build_flag}
    ADT_VER_MAJ=${ver_maj}
    ADT_VER_MIN=${ver_min}
    ADT_VER_PATCH=${ver_patch}
    ADT_VER_STRING=${ver_string})

target_include_directories(
  adt
  PRIVATE include
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/export>
         $<INSTALL_INTERFACE:include>)

target_link_libraries(adt )

target_compile_features(
  adt
  PUBLIC
  cxx_nullptr
  cxx_lambdas
  cxx_auto_type
  cxx_static_assert)

configure_file(
  cmake/adtConfig.cmake.in
  ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/adtConfig.cmake @ONLY)

# INSTALL

export(
  TARGETS adt
  FILE ${PROJECT_BINARY_DIR}/adtTargets.cmake)

export(PACKAGE adt)

# install configs

install(
  TARGETS adt
  EXPORT adtTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include)

install(
  DIRECTORY export/
  DESTINATION include)

set(cmake_config_dir lib/cmake/adt)

install(
  FILES ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/adtConfig.cmake
  DESTINATION ${cmake_config_dir}
  COMPONENT dev)

install(
  EXPORT adtTargets
  DESTINATION ${cmake_config_dir}
  COMPONENT dev)

install(
  FILES ${PROJECT_SOURCE_DIR}/LICENSE
  DESTINATION share/adt)

# TESTS

include(tests.cmake)

# Packaging

set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_VENDOR "twiddle-bits")
set(CPACK_PACKAGE_CONTACT "twiddlebits.ak@gmail.com")
set(CPACK_STRIP_FILES 1)
include(CPack)
