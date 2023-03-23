pegasus_cxx_test(test_LogiqxProvider)

qtquick_compiler_add_resources(TEST_RESOURCES data/data.qrc)
target_sources(test_LogiqxProvider PRIVATE ${TEST_RESOURCES})
target_include_directories(test_LogiqxProvider PRIVATE "${PROJECT_SOURCE_DIR}/src/backend")
