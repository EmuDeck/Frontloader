pegasus_cxx_test(test_PegasusProvider)

qtquick_compiler_add_resources(TEST_RESOURCES data/data.qrc)
target_sources(test_PegasusProvider PRIVATE ${TEST_RESOURCES})
target_include_directories(test_PegasusProvider PRIVATE "${PROJECT_SOURCE_DIR}/src/backend")
