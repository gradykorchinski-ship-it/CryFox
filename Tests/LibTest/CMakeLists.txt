set(TEST_SOURCES
    TestNoCrash.cpp
    TestGenerator.cpp
)

foreach(source IN LISTS TEST_SOURCES)
    cryfox_test("${source}" Test)
endforeach()
