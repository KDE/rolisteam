function(rinstallLib libname comp)
  if(WIN32)
    install(TARGETS ${libname} LIBRARY DESTINATION . COMPONENT ${comp} ARCHIVE DESTINATION . COMPONENT ${comp} RUNTIME DESTINATION . COMPONENT ${comp} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${comp})
  elseif(UNIX AND NOT APPLE)
    install(TARGETS ${libname} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${comp} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${comp})#RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    #install(TARGETS ${libname} LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${comp}_dep)
  endif()
endfunction()

function(rinstallRT libname comp)
  if(WIN32)
    install(TARGETS ${libname} LIBRARY DESTINATION . COMPONENT ${comp} ARCHIVE DESTINATION . COMPONENT ${comp} RUNTIME DESTINATION . COMPONENT ${comp} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${comp})
  elseif(UNIX AND NOT APPLE)
    install(TARGETS ${libname} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${comp} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${comp} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${comp})
  endif()
endfunction()

