####### CPack ###############################

set(CPACK_GENERATOR "DEB;RPM;STGZ")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
#  We need to compile python scripts on the installation  host.
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dilawar Singh")
set(CPACK_PACKAGE_CONTACT "dilawars@ncbs.res.in")
#set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MOOSE, The Neural Simulator")
set(CPACK_PACKAGE_VERSION "3.0.1")
set(CPACK_PACKAGE_VERSION_MAJOR "3")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_VENDOR "NCBS Bangalore")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dilawar Singh <dilawars@ncbs.res.in>")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${PKGARCH}")
set(CPACK_PACKAGE_FILE_NAME "moose-${CPACK_PACKAGE_VERSION}-Linux-${PKGARCH}")
set(CPACK_STRIP_FILES moose)

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_DEPENDS 
    "openmpi-bin
    , python-matplotlib
    , python-qt4, openscenegraph
    , python-tornado, python-suds, python-nose, python-setuptools
    , libxml2"
)

set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    "${DEBIAN_POSTINST_PATH};${DEBIAN_PRERM_PATH};"
    )

## RPM 
set(CPACK_RPM_PACKAGE_ARCHITECTURE "${RPMPKGARCH}")
set(CPACK_RPM_SPEC_INSTALL_POST "/bin/true")
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${RPM_POSTINST_PATH}")
set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${RPM_PRERM_PATH}")

# This has to be a single line.
set(CPACK_RPM_PACKAGE_REQUIRES
    "openmpi-devel python-matplotlib-qt4 python-qt4  python-tornado python-suds python-nose python-setuptools libxml2"
    )

# A workaround in RPM
set(CPACK_RPM_SPEC_MORE_DEFINE "%define ignore \#")
set(CPACK_RPM_USER_FILELIST "%ignore /" "%ignore /usr" "%ignore /usr/share" 
    "%ignore /usr/share/applications"
    "%ignore /tmp"
    )

## Apple support
if(APPLE)

    MESSAGE("++ Mac BUNDLE ON for ${CMAKE_PROJECT_NAME}")
    ## Mac bundle
    SET(BUNDLE_NAME ${CMAKE_PROJECT_NAME})
    SET(CPACK_BUNDLE_NAME ${BUNDLE_NAME})

    if(DEBUG)
        SET_TARGET_PROPERTIES(moose.bin PROPERTIES MACOSX_BUNDLE TRUE)
    endif(DEBUG)
    SET_TARGET_PROPERTIES(_moose PROPERTIES MACOSX_BUNDLE TRUE)

endif(APPLE)

include(CPack)
