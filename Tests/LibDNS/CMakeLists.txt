set(TEST_SOURCES
    TestDNSResolver.cpp
)

foreach(source IN LISTS TEST_SOURCES)
    cryfox_test("${source}" LibDNS LIBS LibDNS LibTLS)
endforeach()
