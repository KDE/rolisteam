cmake_minimum_required(VERSION 3.16)

project(rolisteam VERSION 1.10.0 LANGUAGES CXX)

if (MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /experimental:external /external:W0 ")
    set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "/experimental:external /external:I ")
endif()

find_package(ECM REQUIRED NO_MODULE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_CURRENT_BINARY_DIR ON)
set(CMAKE_AUTORCC ON)
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
include(FormattingHelper)

option(CODE_COVERAGE OFF)
option(MANUAL_INSTALL OFF)
option(FORMAT_CODE OFF)
option(BUILD_DICE3D OFF)
option(DEBUG_MODE "enable more debug report" OFF)
option(WARNING_AS_ERROR "Set all warning as error" OFF)
option(GENERATE_TS_FILES "generate all ts file from po" OFF)

if(ANDROID)
    find_package(Qt6 COMPONENTS Core Widgets Gui LinguistTools REQUIRED)
else()
    find_package(Qt6 COMPONENTS Core Widgets Gui LinguistTools WebEngineCore WebEngineWidgets WebEngineQuick REQUIRED)
endif()

include(gitinfo)
get_version_from_git()

configure_file(version.h.in version.h @ONLY)

if (CMAKE_BUILD_TYPE STREQUAL "Release" OR ${GENERATE_TS_FILES} )
    message("[RELEASE] generate ts files from po")
    include(TranslationHelper)
endif()

add_subdirectory(src)
add_subdirectory(translations)


if(EXISTS ${IFW_PATH})
  include(CPackHelper)
endif()

set(documentation README.md HelpMe.md)

configure_file(scripts/generate_coverage.sh.in generate_coverage.sh @ONLY)

if(EXISTS ${IFW_PATH})
set(icons
    resources/rolisteam/logo/rolisteam.ico
    resources/rcse/logo/rcse.ico
    resources/mindmap/logo/mindmap.ico)

install(FILES ${icons} DESTINATION logo COMPONENT rolisteamComp)
endif()

include(KDEInstallDirs6 RESULT_VARIABLE installDir) # OPTIONAL RESULT_VARIABLE installDir

if(NOT "${installDir}" STREQUAL "NOTFOUND")
install(FILES resources/packaging/Linux/flatpak/org.rolisteam.rolisteam.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES resources/packaging/Linux/flatpak/org.rolisteam.rolisteam.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES resources/packaging/Linux/flatpak/org.rolisteam.rolisteam.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)
else()
message("KDEInstallDirs6 not found")
endif()


