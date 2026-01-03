cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)
find_package(Qt6 CONFIG REQUIRED COMPONENTS Core Gui Quick)


qt_add_qml_module(WalkerComponent
    URI Walker
    VERSION 1.0
    SOURCES walkeritem.h walkeritem.cpp
    SOURCES walkerattachedtype.h walkerattachedtype.cpp
)
target_link_libraries(WalkerComponent
    PRIVATE Qt6::Core Qt6::Gui Qt6::Quick
)
