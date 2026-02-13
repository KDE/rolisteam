cmake_minimum_required(VERSION 3.16)

enable_testing(true)
include_directories( test )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_REQUIRED_VERSION "6.3.0")
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Test Core Gui Widgets StateMachine)

add_executable(tst_actiondelegate tst_actiondelegate.cpp ../assets/resources.qrc)
target_link_libraries(tst_actiondelegate PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Gui PRIVATE core qml_views rmindmap diceparser_shared dice3d qml_components charactersheet_widgets charactersheet rwidgets instantmessaging common_core common_widget common_qml utils helper)
target_include_directories(tst_actiondelegate PUBLIC ../helper)
add_test(NAME tst_actiondelegate COMMAND tst_actiondelegate)

add_executable(tst_avatardelegate tst_avatardelegate.cpp ../assets/resources.qrc)
target_link_libraries(tst_avatardelegate PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Gui PRIVATE core qml_components qml_views charactersheet_widgets charactersheet dice3d diceparser_shared rmindmap rwidgets instantmessaging common_core common_widget common_qml utils helper)
target_include_directories(tst_avatardelegate PUBLIC ../helper)
add_test(NAME tst_avatardelegate COMMAND tst_avatardelegate)

add_executable(tst_checkboxdelegate tst_checkbox.cpp ../assets/resources.qrc)
target_link_libraries(tst_checkboxdelegate PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Gui PRIVATE charactersheet core rwidgets instantmessaging common_core common_widget common_qml rmindmap utils helper network)
target_include_directories(tst_checkboxdelegate PUBLIC ../helper)
add_test(NAME tst_checkboxdelegate COMMAND tst_checkboxdelegate)

add_executable(tst_colordelegate tst_color.cpp ../assets/resources.qrc)
target_link_libraries(tst_colordelegate PUBLIC Qt6::Core Qt6::Test Qt6::Widgets Qt6::Gui PRIVATE rmindmap qml_views diceparser_qobject diceparser_shared qml_components charactersheet_widgets charactersheet dice3d rmindmap core rwidgets instantmessaging common_core common_widget common_qml network helper utils helper)
target_include_directories(tst_colordelegate  PUBLIC ../helper ../../libraries/mindmap/include)
add_test(NAME tst_colordelegate COMMAND tst_colordelegate)

add_executable(tst_delegate tst_delegate.cpp ../assets/resources.qrc)
target_link_libraries(tst_delegate PUBLIC Qt6::Core Qt6::Test Qt6::StateMachine PRIVATE core rmindmap qml_views diceparser_qobject diceparser_shared utils qml_components charactersheet_widgets charactersheet dice3d rwidgets instantmessaging common_core common_widget common_qml helper network diceparser_qobject diceparser_shared utils helper)
target_include_directories(tst_delegate  PUBLIC ../helper )
add_test(NAME tst_delegate COMMAND tst_delegate)

add_executable(tst_imagepathitemdelegate tst_imagepathitem.cpp ../assets/resources.qrc)
target_link_libraries(tst_imagepathitemdelegate PUBLIC Qt6::Core Qt6::Test Qt6::StateMachine PRIVATE core rwidgets instantmessaging rmindmap common_core common_widget common_qml helper network diceparser_qobject diceparser_shared utils helper)
target_include_directories(tst_imagepathitemdelegate  PUBLIC ../helper )
add_test(NAME tst_imagepathitemdelegate COMMAND tst_imagepathitemdelegate)

add_executable(tst_statedelegate tst_state.cpp ../assets/resources.qrc)
target_link_libraries(tst_statedelegate PUBLIC Qt6::Core Qt6::Test Qt6::StateMachine PRIVATE core rwidgets instantmessaging rmindmap qml_views diceparser_qobject diceparser_shared utils qml_components charactersheet_widgets charactersheet dice3d common_qml instantmessaging common_core qml_views rmindmap common_core common_widget common_qml helper network diceparser_qobject diceparser_shared utils helper)
target_include_directories(tst_statedelegate  PUBLIC ../helper )
add_test(NAME tst_statedelegate COMMAND tst_statedelegate)


add_executable(tst_taglistdelegate tst_taglist.cpp ../assets/resources.qrc)
target_link_libraries(tst_taglistdelegate PUBLIC Qt6::Core Qt6::Test Qt6::StateMachine PRIVATE core rwidgets instantmessaging rmindmap qml_views qml_components rmindmap qml_views diceparser_qobject diceparser_shared utils qml_components charactersheet_widgets charactersheet dice3d common_qml instantmessaging common_core common_widget common_qml helper network diceparser_qobject diceparser_shared utils helper)
target_include_directories(tst_taglistdelegate  PUBLIC ../helper )
add_test(NAME tst_taglistdelegate COMMAND tst_taglistdelegate)


add_executable(tst_userlistdelegate tst_userlist.cpp ../assets/resources.qrc)
target_link_libraries(tst_userlistdelegate PUBLIC Qt6::Core Qt6::Test Qt6::StateMachine PRIVATE core rwidgets instantmessaging rmindmap common_core common_widget common_qml helper network diceparser_qobject diceparser_shared utils helper rmindmap qml_views diceparser_qobject diceparser_shared utils qml_components charactersheet_widgets charactersheet dice3d common_qml instantmessaging common_core qml_views)
target_include_directories(tst_userlistdelegate  PUBLIC ../helper )
add_test(NAME tst_userlistdelegate COMMAND tst_userlistdelegate)
