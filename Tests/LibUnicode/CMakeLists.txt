set(TEST_SOURCES
    TestDisplayNames.cpp
    TestIDNA.cpp
    TestLocale.cpp
    TestSegmenter.cpp
    TestTimeZone.cpp
    TestUnicodeCharacterTypes.cpp
    TestUnicodeNormalization.cpp
)

foreach(source IN LISTS TEST_SOURCES)
    cryfox_test("${source}" LibUnicode LIBS LibUnicode)
endforeach()
