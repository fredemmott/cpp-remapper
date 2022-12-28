cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
cmake_policy(SET CMP0135 NEW)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(VERSION_BUILD 0 CACHE STRING "Build component of the version number")
project(cpp-remapper VERSION 0.4.0.${VERSION_BUILD} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# - Use ASAN for debug builds
# - statically link to statically link the ASAN runtime
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  add_compile_options(
    "$<$<CONFIG:Debug>:/fsanitize=address>"
    "$<$<CONFIG:Debug>:/MTd>"
  )
endif()

function(add_cppremapper_executable TARGET)
  add_executable(${TARGET} ${ARGN})
  target_link_libraries(
    "${TARGET}"
    PRIVATE
    LibCppRemapper
  )
  add_custom_command(
    TARGET "${TARGET}"
    POST_BUILD
    COMMAND
    "${CMAKE_COMMAND}"
    -E copy
    "$<TARGET_RUNTIME_DLLS:${TARGET}>"
    "$<TARGET_FILE_DIR:${TARGET}>"
  )
  install(TARGETS "${TARGET}")
endfunction()

add_subdirectory(third-party)
add_subdirectory(lib)
add_subdirectory(utilities)
add_subdirectory(tests)

include(legacy_profiles.cmake)
