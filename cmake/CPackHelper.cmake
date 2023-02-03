set(CPACK_GENERATOR "IFW")
set(CPACK_IFW_ROOT ${ifwpath})# /home/renaud/application/other/Qt/Tools/QtInstallerFramework/4.2
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "Rolisteam")
set(CPACK_PACKAGE_NAME "Rolisteam")
set(CPACK_PACKAGE_VENDOR "Rolisteam.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Pen and Paper virtual tabletop Software.")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
#set(CPACK_INSTALL_CMAKE_PROJECTS )
set(CPACK_PACKAGE_HOMEPAGE_URL "https://www.rolisteam.org")
set(CPACK_PACKAGE_CONTACT "rolisteam-org developers <www@rolisteam.org>")
set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources/rolistheme/org.rolisteam.Rolisteam.svg")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_VERSION_MAJOR ${rolisteam_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${rolisteam_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${rolisteam_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
#set(CPACK_IFW_PACKAGE_NAME "Rolisteam")
set(CPACK_IFW_PACKAGE_TITLE "Installer Rolisteam")
set(CPACK_IFW_PACKAGE_PUBLISHER "Rolisteam")
set(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")
set(CPACK_IFW_PACKAGE_WINDOW_ICON "${CMAKE_SOURCE_DIR}/resources/rolistheme/500-symbole.png")
set(CPACK_IFW_PACKAGE_BANNER "${CMAKE_SOURCE_DIR}/resources/rolistheme/ifwbanner.png")
set(CPACK_IFW_VERBOSE ON)
set(CPACK_PACKAGE_EXECUTABLES "")


if(WIN32)
  set(CPACK_IFW_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources/rolistheme/128.ico")
elseif(APPLE)
  set(CPACK_IFW_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/resources/rolistheme/rolisteam.icns")
endif()

if(WIN32)
    include(InstallRequiredSystemLibraries)
    find_program(WINDEPLOYQT windeployqt HINTS "${_qt_bin_dir}")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/deploy-qt-windows.cmake.in" "${CMAKE_CURRENT_SOURCE_DIR}/deploy-qt-windows.cmake" @ONLY)

    set(CPACK_PRE_BUILD_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/deploy-qt-windows.cmake)
endif()
include(CPack)
include(CPackIFW)

cpack_add_component(libraries DISPLAY_NAME "Libraries" DESCRIPTION "Dynamic libs")
cpack_add_component(rolisteamComp DISPLAY_NAME "Rolisteam" DEPENDS libraries)
cpack_add_component(rcseComp DISPLAY_NAME "rcse" DEPENDS libraries)
cpack_add_component(roliserverComp DISPLAY_NAME "Roliserver" DEPENDS libraries)
cpack_add_component(mindmapComp DISPLAY_NAME "RMindMap" DEPENDS libraries)
cpack_add_component(diceComp DISPLAY_NAME "Dice CLI" DEPENDS libraries)


cpack_ifw_configure_component(libraries
    DESCRIPTION
        "Dynamic libs"
    SORTING_PRIORITY 1000
    VERSION ${PROJECT_VERSION}
    LICENSES "GPLv2" "${CMAKE_SOURCE_DIR}/COPYING.txt"
    DEFAULT "true"
)

cpack_ifw_configure_component(rolisteamComp
    DESCRIPTION
        "Main application to play TTRPG with friends"
    SORTING_PRIORITY 1000
    VERSION ${PROJECT_VERSION}
    LICENSES "GPLv2" "${CMAKE_SOURCE_DIR}/COPYING.txt"
    DEFAULT "true"
)

cpack_ifw_configure_component(rcseComp
    DESCRIPTION
        "Rolisteam CharacterSheet Editor"
    SORTING_PRIORITY 1000
    VERSION ${PROJECT_VERSION}
    LICENSES "GPLv2" "${CMAKE_SOURCE_DIR}/COPYING.txt"
    DEFAULT "true"
)
#
#cpack_ifw_configure_component(roliserverComp
#    DESCRIPTION
#        "Standalone and headless rolisteam server"
#    SORTING_PRIORITY 1000
#    VERSION ${PROJECT_VERSION}
#    LICENSES "GPLv2" "${CMAKE_SOURCE_DIR}/COPYING.txt"
#    DEFAULT "false"
#)
#
#cpack_ifw_configure_component(mindmapComp
#  DESCRIPTION
#      "Standalone mindmap application"
#  SORTING_PRIORITY 1000
#  VERSION ${PROJECT_VERSION}
#  LICENSES "GPLv3" "${CMAKE_SOURCE_DIR}/COPYING.txt"
#  DEFAULT "false"
#)
#
#cpack_ifw_configure_component(diceComp
#    DESCRIPTION
#        "Dice CLI application"
#    SORTING_PRIORITY 1000
#    VERSION ${PROJECT_VERSION}
#    LICENSES "GPLv3" "${CMAKE_SOURCE_DIR}/COPYING.txt"
#    DEFAULT "false"
#)
