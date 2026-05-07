cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Concurrent StateMachine)

add_executable(tst_widgets tst_widgets.cpp)
target_link_libraries(tst_widgets PUBLIC Qt6::Core Qt6::Test PRIVATE core rwidgets instantmessaging rmindmap qml_views diceparser_qobject diceparser_shared qml_components charactersheet_widgets charactersheet dice3d common_qml instantmessaging common_core common_core common_widget common_qml charactersheet_widgets helper utils)
target_include_directories(tst_widgets  PUBLIC ../helper )
add_test(NAME tst_widgets COMMAND tst_widgets)



add_executable(tst_dialogs tst_dialogs.cpp)
target_link_libraries(tst_dialogs PUBLIC Qt6::Core Qt6::Test Qt6::Concurrent Qt6::StateMachine PRIVATE core rwidgets instantmessaging rmindmap network qml_views diceparser_qobject diceparser_shared qml_components charactersheet_widgets charactersheet dice3d common_qml instantmessaging common_core common_core common_widget common_qml charactersheet_widgets helper utils)
target_include_directories(tst_dialogs  PUBLIC ../helper ${CMAKE_BINARY_DIR})
add_test(NAME tst_dialogs COMMAND tst_dialogs)
