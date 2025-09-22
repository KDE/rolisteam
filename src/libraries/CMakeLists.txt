add_subdirectory(inja)
add_subdirectory(utils)
#add_subdirectory(data)
add_subdirectory(qml_views)
add_subdirectory(diceparser)
add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(qml_components)
add_subdirectory(mindmap)

add_subdirectory(instantmessaging)

add_subdirectory(dicephysics)

if(NOT ANDROID)
    add_subdirectory(charactersheet)
    add_subdirectory(rwidgets)
endif()
