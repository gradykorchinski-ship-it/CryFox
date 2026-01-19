set(TEST_SOURCES
    TestThread.cpp
)

foreach(source IN LISTS TEST_SOURCES)
    cryfox_test("${source}" LibThreading LIBS LibThreading)
endforeach()
