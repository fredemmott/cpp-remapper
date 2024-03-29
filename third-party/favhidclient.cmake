include(FetchContent)
find_package(Git)

set(FAVHID_REVISION "88f3f8f874b5587f473dd9ae78cb7abb7076d669")
FetchContent_Declare(
    favhidclient
    GIT_REPOSITORY https://github.com/fredemmott/favhid-client.git
    GIT_TAG main
    PATCH_COMMAND "${GIT_EXECUTABLE}" reset --hard "${FAVHID_REVISION}"
    EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(favhidclient)

# Build against the same version. Generally a good idea, but especially needed
# for /await compat with the obsolete toolchain on github actions
target_link_libraries(favhid PUBLIC ThirdParty-CppWinRT)

add_library(ThirdParty-FAVHIDClient ALIAS favhid)