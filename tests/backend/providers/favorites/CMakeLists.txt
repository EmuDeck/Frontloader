pegasus_cxx_test(test_FavoriteDB)

qtquick_compiler_add_resources(TEST_RESOURCES data/data.qrc)
target_sources(test_FavoriteDB PRIVATE ${TEST_RESOURCES})
target_include_directories(test_FavoriteDB PRIVATE "${PROJECT_SOURCE_DIR}/src/backend")
