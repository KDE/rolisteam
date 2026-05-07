cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core)

add_executable(tst_vmapupdater tst_vmapupdatertest.cpp)
target_link_libraries(tst_vmapupdater PUBLIC Qt6::Core Qt6::Test PRIVATE core instantmessaging rmindmap qml_views diceparser_qobject diceparser_shared utils qml_components charactersheet_widgets charactersheet dice3d common_qml instantmessaging common_core common_core common_widget common_qml rmindmap network)
target_include_directories(tst_vmapupdater  PUBLIC ../helper ../../libraries/mindmap/include)
add_test(NAME tst_vmapupdater COMMAND tst_vmapupdater)

add_executable(tst_genericupdater tst_genericupdatertest.cpp)
target_link_libraries(tst_genericupdater PUBLIC Qt6::Core Qt6::Test PRIVATE core instantmessaging rmindmap qml_views diceparser_qobject helper diceparser_shared utils qml_components charactersheet_widgets charactersheet dice3d common_qml instantmessaging common_core common_core common_widget common_qml rmindmap network)
target_include_directories(tst_genericupdater  PUBLIC ../helper ../../libraries/mindmap/include)
add_test(NAME tst_genericupdater COMMAND tst_genericupdater)

add_executable(tst_webpageupdater tst_webpagetest.cpp)
target_link_libraries(tst_webpageupdater PUBLIC Qt6::Core Qt6::Test PRIVATE core instantmessaging rmindmap qml_views diceparser_qobject helper diceparser_shared utils qml_components charactersheet_widgets charactersheet dice3d common_qml instantmessaging common_core common_core common_widget common_qml rmindmap network)
target_include_directories(tst_webpageupdater  PUBLIC ../helper ../../libraries/mindmap/include)
add_test(NAME tst_webpageupdater COMMAND tst_webpageupdater)