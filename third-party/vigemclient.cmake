include(FetchContent)

FetchContent_Declare(
  FC_ViGEmClient
  URL "https://github.com/ViGEm/ViGEmClient/archive/refs/tags/v1.21.222.0.zip"
  URL_HASH "SHA256=4c61f0a82ea1153e271fe760712a97f4edaf64ad5f4244aca78eead74a3749b9"
)
FetchContent_MakeAvailable(FC_ViGEmClient)

add_library(ThirdParty-ViGEmClient ALIAS ViGEmClient)
