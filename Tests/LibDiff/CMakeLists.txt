set(TEST_SOURCES
    TestDiff.cpp
)

foreach(source IN LISTS TEST_SOURCES)
    cryfox_test("${source}" LibDiff LIBS LibDiff)
endforeach()
