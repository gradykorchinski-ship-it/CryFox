set(SOURCES
    ConfigLoader.cpp
    SupabaseClient.cpp
    LocalAuthManager.cpp
)

cryfox_lib(LibAuth auth)
target_link_libraries(LibAuth PRIVATE LibCore LibCrypto)
target_link_libraries(LibAuth PRIVATE CURL::libcurl)
find_package(CURL REQUIRED)
