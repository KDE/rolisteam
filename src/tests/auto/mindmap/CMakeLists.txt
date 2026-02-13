cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_mindmap tst_mindmap.cpp ../assets/resources.qrc)
include(Sanitizers)
enable_sanitizers(tst_mindmap)
include(CompilerWarnings)
set_project_warnings(tst_mindmap)
target_link_libraries(tst_mindmap PUBLIC Qt6::Core Qt6::Test PRIVATE core rmindmap  rmindmap qml_views diceparser_qobject diceparser_shared utils qml_components charactersheet_widgets charactersheet dice3d rwidgets instantmessaging common_core common_widget common_qml utils helper network)
target_include_directories(tst_mindmap  PUBLIC ../helper )
add_test(NAME tst_mindmap COMMAND tst_mindmap)
