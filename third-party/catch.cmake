add_library(
  ThirdParty-Catch2
  INTERFACE
)

target_include_directories(
  ThirdParty-Catch2
  INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/catch2"
)
