include(FetchContent)

FetchContent_Declare(
  fcvjoysdk
  URL "https://github.com/njz3/vJoy/releases/download/v2.2.1.1/vJoy-2.2.1.1-SDK.zip"
  URL_HASH "SHA256=49afe972e4ecddde86736ccb17463095fb10faea484d40349b88d5e4f416398f"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)
FetchContent_MakeAvailable(fcvjoysdk)

set(VJoy_LIBDIR "${fcvjoysdk_SOURCE_DIR}/lib")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(VJoy_LIBDIR "${VJoy_LIBDIR}/amd64")
else()
  message(WARNING "Use 64-bit builds, it's 2022")
endif()

add_library(ThirdParty-VJoy SHARED IMPORTED GLOBAL)

set_target_properties(
  ThirdParty-VJoy
  PROPERTIES
  IMPORTED_LOCATION "${VJoy_LIBDIR}/vjoyInterface.dll"
  IMPORTED_IMPLIB "${VJoy_LIBDIR}/vjoyInterface.lib"
)

target_include_directories(
  ThirdParty-VJoy
  INTERFACE
  "${fcvjoysdk_SOURCE_DIR}/inc"
)
