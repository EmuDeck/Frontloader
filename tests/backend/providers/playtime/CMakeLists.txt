pegasus_cxx_test(test_Playtime)

qtquick_compiler_add_resources(TEST_RESOURCES data.qrc)
target_sources(test_Playtime PRIVATE ${TEST_RESOURCES})
target_include_directories(test_Playtime PRIVATE "${PROJECT_SOURCE_DIR}/src/backend")
