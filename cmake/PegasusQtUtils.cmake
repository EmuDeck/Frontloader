# NOTE: This is a macro because `find_package` variables would go
# out of scope at the end of functions
macro(pegasus_require_qt)
    cmake_parse_arguments(QTREQ
        ""
        ""
        COMPONENTS
        ${ARGN})


    find_package(Qt5 5.15 COMPONENTS ${QTREQ_COMPONENTS} REQUIRED)
endmacro()
