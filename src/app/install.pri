include($${TOP_SRCDIR}/src/deployment_vars.pri)

unix:!macx {
    md.files += \
        $${TOP_SRCDIR}/LICENSE.md \
        $${TOP_SRCDIR}/README.md
    md.path = $${INSTALLDIR}
    OTHER_FILES += $${md.files}
    INSTALLS += md
    target.path = $${INSTALLDIR}
}


!isEmpty(target.path): INSTALLS += target
