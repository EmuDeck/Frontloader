pegasus_cxx_test(bench_PegasusProvider)

qtquick_compiler_add_resources(TEST_RESOURCES data/data.qrc)
target_sources(bench_PegasusProvider PRIVATE ${TEST_RESOURCES})
target_include_directories(bench_PegasusProvider PRIVATE "${PROJECT_SOURCE_DIR}/src/backend")
