set(SOURCES
    ConnectionInfo.cpp
    Impl/WebSocketImpl.cpp
    Impl/WebSocketImplSerenity.cpp
    WebSocket.cpp
)

cryfox_lib(LibWebSocket websocket)
target_link_libraries(LibWebSocket PRIVATE LibCore LibCrypto LibHTTP LibTLS LibURL LibDNS)
