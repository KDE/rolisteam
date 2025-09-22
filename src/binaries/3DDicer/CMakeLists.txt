cmake_minimum_required(VERSION 3.16)

project(3DDicer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Quick3D Quick3DPhysics)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(app3DDicer
    main.cpp
)

qt_add_resources(app3DDicer assets
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
)

set_source_files_properties(Theme.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(app3DDicer
    URI treeDicer
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES dicemaincontroller.h dicemaincontroller.cpp
        SOURCES rollmodel.h rollmodel.cpp
        QML_FILES Panel.qml Theme.qml DiceRollPage.qml SettingsPage.qml SheetPage.qml MacrosPage.qml PhysicsDicePage.qml AliasesPage.qml
        SOURCES settingcontroller.h settingcontroller.cpp
        SOURCES propertiesmodel.h propertiesmodel.cpp
        SOURCES session.h session.cpp
        SOURCES sessionmodel.h sessionmodel.cpp
        SOURCES macromodel.cpp macromodel.h

)



# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(app3DDicer PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.app3DDicer
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(app3DDicer
PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Concurrent
    Qt::Quick3D
    Qt::Quick3DPhysics
    diceparser_shared
    dice3d
    core
)
target_link_libraries(app3DDicer PRIVATE Qt6::Core)
target_link_libraries(app3DDicer PRIVATE Qt6::Core)


include(GNUInstallDirs)
install(TARGETS app3DDicer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
