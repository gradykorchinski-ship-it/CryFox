set(SOURCES
    HistoryManager.cpp
    PasswordVault.cpp
)

cryfox_lib(LibBrowser browser)
target_link_libraries(LibBrowser PRIVATE LibCore LibURL LibAuth LibCrypto)
target_link_libraries(LibBrowser PUBLIC sqlite3)
