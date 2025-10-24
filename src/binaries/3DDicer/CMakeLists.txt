cmake_minimum_required(VERSION 3.20)

project(dicelyVerse VERSION 7 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Quick3D QuickControls2 Quick3DPhysics LinguistTools)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup(REQUIRES 6.8 I18N_SOURCE_LANGUAGE en I18N_TRANSLATED_LANGUAGES de fr it es pt)

include(gitinfo)
get_version_from_git()


set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 7)



configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/dicelyverse/version.h
)


qt_add_executable(dicelyVerse
    main.cpp
    model/dicealiasmodel.cpp
    model/dicealiasmodel.h
    worker/fileserializer.cpp
    worker/fileserializer.h
)

qt_add_resources(dicelyVerse assets
PREFIX
"/"
FILES
    assets/dice.svg
    assets/diceroll.svg
    assets/send.svg
    assets/send2.svg
    assets/send3.svg
    assets/settings.svg
    assets/settings2.svg
    assets/sheet.svg
    assets/sheet2.svg
    assets/redo.svg
    assets/plus.svg
    assets/plus2.svg
    assets/plus3.svg
    assets/aliases.svg
    assets/bookmark.svg
    assets/edit.svg
    assets/check.svg
    assets/remove.svg
    assets/menu.svg
    assets/selectall.svg
    assets/selectRect.svg
    assets/external.svg
    assets/trashbin.svg
    assets/lock.svg
)

set_property(TARGET dicelyVerse PROPERTY QT_EXCLUDE_SOURCES_FROM_TRANSLATION "assets/*.svg")

set_source_files_properties(Theme.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_resources(dicelyVerse CustomStyle
    PREFIX "/qt/qml"
    FILES
        DiceStyle/Button.qml
        DiceStyle/Label.qml
        DiceStyle/Pane.qml
        DiceStyle/SwipeDelegate.qml
        DiceStyle/qmldir
        DiceStyle/TextField.qml
        DiceStyle/ItemDelegate.qml
        DiceStyle/GroupBox.qml
        DiceStyle/ToolButton.qml
        DiceStyle/Switch.qml
        DiceStyle/TabButton.qml
        DiceStyle/ComboBox.qml
        DiceStyle/PageIndicator.qml
)

qt_add_qml_module(dicelyVerse
    URI dicely
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES dicemaincontroller.h dicemaincontroller.cpp
        SOURCES rollmodel.h rollmodel.cpp
        QML_FILES Panel.qml  DiceRollPage.qml SettingsPage.qml SheetPage.qml MacrosPage.qml PhysicsDicePage.qml AliasesPage.qml
        SOURCES settingcontroller.h settingcontroller.cpp
        SOURCES propertiesmodel.h propertiesmodel.cpp
        SOURCES session.h session.cpp
        SOURCES sessionmodel.h sessionmodel.cpp
        SOURCES macromodel.cpp macromodel.h
        RESOURCES android/AndroidManifest.xml android/build.gradle android/res/values/libs.xml android/res/xml/qtprovider_paths.xml
        RESOURCES version.h.in
        QML_FILES ExpansionPanel.qml
        QML_FILES ProfileSelector.qml
        SOURCES themecontroller.h themecontroller.cpp
)

qt_add_translations(dicelyVerse
    TS_FILE_BASE qml
    TS_FILE_DIR i18n
    RESOURCE_PREFIX /qt/qml/dicely/i18n
    TS_OUTPUT_DIRECTORY translations
)

set_target_properties(dicelyVerse PROPERTIES
    QT_ANDROID_VERSION_NAME "Auguste"
    QT_ANDROID_VERSION_CODE "${PROJECT_VERSION}"
)
target_include_directories(dicelyVerse PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(dicelyVerse PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.app3DDicer
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
set_target_properties(dicelyVerse PROPERTIES
       QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
   )
target_link_libraries(dicelyVerse
PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Concurrent
    Qt::Quick3D
    Qt::Quick3DPhysics
    Qt6::QuickControls2
    diceparser_shared
    dice3d
)

target_link_libraries(dicelyVerse PRIVATE Qt6::Core)
target_link_libraries(dicelyVerse PRIVATE Qt6::Core)


include(GNUInstallDirs)
install(TARGETS dicelyVerse
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
