function(rinstallLib libname comp)
  if(WIN32)
    install(TARGETS ${libname} LIBRARY DESTINATION . COMPONENT ${comp} ARCHIVE DESTINATION . COMPONENT ${comp} RUNTIME DESTINATION . COMPONENT ${comp} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/headers COMPONENT ${comp})
  elseif(UNIX AND NOT APPLE)
    install(TARGETS ${libname} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${comp} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rolisteam COMPONENT ${comp})#RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  endif()
endfunction()

function(rinstallRT libname comp)
  if(WIN32)
    install(TARGETS ${libname} LIBRARY DESTINATION . COMPONENT ${comp} ARCHIVE DESTINATION . COMPONENT ${comp} RUNTIME DESTINATION . COMPONENT ${comp} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/headers COMPONENT ${comp})
  elseif(UNIX AND NOT APPLE)
    install(TARGETS ${libname} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${comp} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${comp} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rolisteam COMPONENT ${comp})
  endif()
endfunction()


function(setupIcon dirRoot name)
if(UNIX AND NOT APPLE)
  file(GLOB subDirs RELATIVE ${dirRoot}  ${dirRoot}/*)
  foreach(dir ${subDirs})
    IF(IS_DIRECTORY ${dirRoot}/${dir})
      file(GLOB icons RELATIVE ${dirRoot}/${dir}  ${dirRoot}/${dir}/${name}.*)
      foreach(icon ${icons})
        install(
            FILES "${dirRoot}/${dir}/${icon}"
            DESTINATION "share/icons/hicolor/${dir}/apps/${icon}")
      endforeach()
    endif()
  endforeach()
endif()
endfunction()

