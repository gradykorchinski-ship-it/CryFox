set(TEST_SOURCES
    TestTextDecoders.cpp
    TestTextEncoders.cpp
)

foreach(source IN LISTS TEST_SOURCES)
    cryfox_test("${source}" LibTextCodec LIBS LibTextCodec)
endforeach()
