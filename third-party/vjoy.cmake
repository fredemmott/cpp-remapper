add_library(
  ThirdParty-VJoy
  SHARED IMPORTED GLOBAL
)

set(VJoy_LIBDIR "${CMAKE_CURRENT_SOURCE_DIR}/vJoy/SDK/lib")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(VJoy_LIBDIR "${VJoy_LIBDIR}/amd64")
else()
  message(WARNING "Use 64-bit builds, it's 2022")
endif()

set_target_properties(
  ThirdParty-VJoy
  PROPERTIES
  IMPORTED_LOCATION "${VJoy_LIBDIR}/vjoyInterface.dll"
  IMPORTED_IMPLIB "${VJoy_LIBDIR}/vjoyInterface.lib"
)

target_include_directories(
  ThirdParty-VJoy
  INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/vJoy"
  "${CMAKE_CURRENT_SOURCE_DIR}/vJoy/SDK/inc"
)
