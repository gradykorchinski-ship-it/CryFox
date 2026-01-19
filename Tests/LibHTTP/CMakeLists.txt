set(TEST_SOURCES
    TestHTTPUtils.cpp
)

foreach(source IN LISTS TEST_SOURCES)
    cryfox_test("${source}" LibWeb LIBS LibHTTP)
endforeach()
