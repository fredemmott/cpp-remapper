include(ExternalProject)

ExternalProject_Add(
  EP_CppWinRT
  URL "https://github.com/microsoft/cppwinrt/releases/download/2.0.220912.1/Microsoft.Windows.CppWinRT.2.0.220912.1.nupkg"
  URL_HASH "SHA256=d57e487b4e35d33ac7e808d4b63a664b99802047a8044eadf130e8048668252a"

  CONFIGURE_COMMAND ""
  BUILD_COMMAND
  "<SOURCE_DIR>/bin/cppwinrt.exe"
  -in "${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}"
  -output "<BINARY_DIR>/include"
  INSTALL_COMMAND ""

  EXCLUDE_FROM_ALL
)
message(STATUS "Windows target version: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")

ExternalProject_Get_property(EP_CppWinRT SOURCE_DIR)
ExternalProject_Get_property(EP_CppWinRT BINARY_DIR)

add_library(ThirdParty-CppWinRT INTERFACE)
add_dependencies(ThirdParty-CppWinRT EP_CppWinRT)
target_link_libraries(ThirdParty-CppWinRT INTERFACE "WindowsApp")
target_include_directories(ThirdParty-CppWinRT INTERFACE "${BINARY_DIR}/include")
