cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_musicmodel tst_musicmodel.cpp ../assets/resources.qrc)
target_link_libraries(tst_musicmodel PUBLIC Qt6::Core Qt6::Test PRIVATE core rwidgets rmindmap qml_views diceparser_qobject diceparser_shared utils qml_components charactersheet_widgets charactersheet dice3d instantmessaging common_core common_widget common_qml helper)
target_include_directories(tst_musicmodel  PUBLIC ../helper ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME tst_musicmodel COMMAND tst_musicmodel)
