include(FetchContent)
find_package(Git)
FetchContent_Declare(
    favhidclient
    GIT_REPOSITORY https://github.com/fredemmott/favhid-client.git
    GIT_TAG main
    UPDATE_COMMAND
    "${GIT_EXECUTABLE}" reset --hard e3d4f8a471c73ef7da3081544850b7604fb9c764
    EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(favhidclient)

# Build against the same version. Generally a good idea, but especially needed
# for /await compat with the obsolete toolchain on github actions
target_link_libraries(favhid PUBLIC ThirdParty-CppWinRT)

add_library(ThirdParty-FAVHIDClient ALIAS favhid)