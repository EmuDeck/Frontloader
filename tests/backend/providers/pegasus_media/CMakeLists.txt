pegasus_cxx_test(test_PegasusMediaProvider)

qtquick_compiler_add_resources(TEST_RESOURCES data/data.qrc)
target_sources(test_PegasusMediaProvider PRIVATE ${TEST_RESOURCES})
target_include_directories(test_PegasusMediaProvider PRIVATE "${PROJECT_SOURCE_DIR}/src/backend")
