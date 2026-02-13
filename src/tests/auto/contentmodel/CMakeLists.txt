cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

find_package(Qt6 CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_contentmodel tst_contentmodel.cpp)
target_link_libraries(tst_contentmodel PUBLIC Qt6::Core Qt6::Test PRIVATE core rmindmap qml_views diceparser_qobject diceparser_shared utils qml_components charactersheet_widgets charactersheet dice3d rwidgets instantmessaging common_core common_widget common_qml)
add_test(NAME tst_contentmodel COMMAND tst_contentmodel)
