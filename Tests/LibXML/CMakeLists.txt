set(TEST_SOURCES
    TestParser.cpp
)

foreach(source IN LISTS TEST_SOURCES)
    cryfox_test("${source}" LibXML LIBS LibXML)
endforeach()
