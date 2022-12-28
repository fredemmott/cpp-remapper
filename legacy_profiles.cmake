option(
  BUILD_LEGACY_PROFILES
  "build profiles/ subdirectory"
  OFF
)

if(NOT BUILD_LEGACY_PROFILES)
  return()
endif()

file(GLOB LEGACY_PROFILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/profiles/*.cpp")

foreach(PROFILE_SOURCE IN LISTS LEGACY_PROFILES)
  get_filename_component(PROFILE_NAME "${PROFILE_SOURCE}" NAME_WLE)
  message(STATUS "Found legacy profile '${PROFILE_NAME}'")
  set(TARGET "Legacy-Profile-${PROFILE_NAME}")
  add_cppremapper_executable(
    "${TARGET}"
    "${PROFILE_SOURCE}"
  )
  set_target_properties(
    "${TARGET}"
    PROPERTIES
    OUTPUT_NAME "${PROFILE_NAME}"
  )
  target_link_libraries("${TARGET}" PRIVATE LibCppRemapper-compat)
endforeach()
