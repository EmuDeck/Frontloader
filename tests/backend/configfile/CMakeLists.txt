pegasus_cxx_test(test_ConfigFile)

qtquick_compiler_add_resources(TEST_RESOURCES configfile.qrc)
target_sources(test_ConfigFile PRIVATE ${TEST_RESOURCES})
target_include_directories(test_ConfigFile PRIVATE "${PROJECT_SOURCE_DIR}/src/backend")
