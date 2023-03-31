include(PegasusQtUtils)
pegasus_require_qt(COMPONENTS
        Core
        X11Extras
        Qml
        QmlImportScanner
        LinguistTools
        )


# Find TS files, compile them to QM and generate a QRC file collecting them

file(GLOB PEGASUS_TS_FILES LIST_DIRECTORIES false "${PROJECT_SOURCE_DIR}/lang/pegasus_*.ts")
qt_add_translation(PEGASUS_QM_FILES "${PEGASUS_TS_FILES}")

set(PEGASUS_LOCALE_QRC "${CMAKE_CURRENT_BINARY_DIR}/locales.qrc")
file(WRITE "${PEGASUS_LOCALE_QRC}" "<!DOCTYPE RCC><RCC version=\"1.0\"><qresource prefix=\"i18n\">\n")

foreach (QM_FILE IN LISTS PEGASUS_QM_FILES)
    get_filename_component(QM_FILENAME "${QM_FILE}" NAME)
    file(APPEND "${PEGASUS_LOCALE_QRC}" "<file>${QM_FILENAME}</file>\n")
endforeach ()

file(APPEND "${PEGASUS_LOCALE_QRC}" "</qresource></RCC>")
qt_add_resources(PEGASUS_LOCALE_SRC "${PEGASUS_LOCALE_QRC}")

add_library(frontloader-locales "${PEGASUS_LOCALE_SRC}")

find_package(X11 REQUIRED)

# Main runtime

if ("${QT_VERSION_MAJOR}" GREATER_EQUAL 6)
    qt_add_executable(frontloader MANUAL_FINALIZATION WIN32 main.cpp)
else ()
    add_executable(frontloader WIN32 main.cpp)
endif ()
target_include_directories(frontloader PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
        ${X11_INCLUDE_DIR}
        )
target_link_libraries(frontloader PUBLIC
        Qt::Core
        Qt::X11Extras
        X11
        frontloader-assets
        frontloader-backend
        frontloader-locales
        frontloader-qml
        )
qt_import_qml_plugins(frontloader)

include(PegasusCommonProps)
pegasus_add_common_props_optimized(frontloader)

# TODO: Workaround for asset differences for now
target_compile_definitions(frontloader PRIVATE PEGASUS_USING_CMAKE)

if (PEGASUS_ENABLE_APNG)
    target_compile_definitions(frontloader PUBLIC WITH_APNG_SUPPORT)
endif ()

if ("${QT_VERSION_MAJOR}" GREATER_EQUAL 6)
    qt_finalize_executable(frontloader)
endif ()


# Install

include(PegasusTargetPlatform)
include(GNUInstallDirs)

macro(pegasus_set_installdir var gnu_default)
    set(${var} "${PEGASUS_${var}}")
    if (NOT ${var})
        set(${var} "${PEGASUS_INSTALLDIR}")
    endif ()
    if (NOT ${var})
        set(${var} "${gnu_default}")
    endif ()
endmacro()

pegasus_set_installdir(INSTALL_BINDIR "/opt/frontloader")
pegasus_set_installdir(INSTALL_DOCDIR "/opt/frontloader")

if (INSTALL_BINDIR)
    install(TARGETS frontloader RUNTIME DESTINATION "${INSTALL_BINDIR}")
endif ()

if (INSTALL_DOCDIR)
    install(FILES
            "${PROJECT_SOURCE_DIR}/README.md"
            "${PROJECT_SOURCE_DIR}/LICENSE.md"
            DESTINATION "${INSTALL_DOCDIR}"
            )
endif ()

# Print config

include(PegasusPrettyPrint)
pegasus_print_box(
        TITLE "App Settings"
        CONTENT
        "Build type: ${CMAKE_BUILD_TYPE}"
        "Git revision: ${PEGASUS_GIT_REVISION}"
        "Git date: ${PEGASUS_GIT_DATE}"
        "Link to static stdc++: ${PEGASUS_STATIC_CXX}"
        "Allow link-time optimizations: ${PEGASUS_ENABLE_LTO}"
        "Deployment paths:"
        "  Binaries:          ${INSTALL_BINDIR}"
        "  License/Readme:    ${INSTALL_DOCDIR}"
)
