cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_unitmodel tst_unitmodel.cpp)
target_link_libraries(tst_unitmodel PUBLIC Qt6::Core Qt6::Test PRIVATE core rwidgets instantmessaging rmindmap qml_views diceparser_qobject diceparser_shared utils qml_components charactersheet_widgets charactersheet dice3d common_qml instantmessaging common_core common_core common_widget common_qml charactersheet_widgets)
add_test(NAME tst_unitmodel COMMAND tst_unitmodel)
