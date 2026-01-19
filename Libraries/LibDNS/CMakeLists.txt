set(SOURCES
    Message.cpp
)

cryfox_lib(LibDNS dns EXPLICIT_SYMBOL_EXPORT)
target_link_libraries(LibDNS PRIVATE LibCore PUBLIC LibCrypto)
