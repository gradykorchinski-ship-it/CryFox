cryfox_test(test-value-js.cpp LibJS LIBS LibJS LibUnicode)

cryfox_testjs_test(test-js.cpp test-js LIBS LibGC)
set_tests_properties(test-js PROPERTIES ENVIRONMENT CRYFOX_SOURCE_DIR=${CRYFOX_PROJECT_ROOT})
