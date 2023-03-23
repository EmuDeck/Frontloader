pegasus_cxx_test(test_Themes)
target_link_libraries(test_Themes PRIVATE pegasus-qml)
target_include_directories(test_Themes PRIVATE "${PROJECT_SOURCE_DIR}/src/backend")
