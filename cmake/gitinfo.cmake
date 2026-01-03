function(get_version_from_git)
    find_package(Git QUIET)
    if(NOT Git_FOUND)
        message(WARNING "Git not found")
        return()
    endif()

    execute_process(
        COMMAND ${GIT_EXECUTABLE} log --date=format:%Y-%m-%d -n 1 --pretty="%ad"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE GIT_RESULT
    )

    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short=8 HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_SHA1
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE GIT_RSHA1
    )
    message("Info: ${CMAKE_SOURCE_DIR}: ${GIT_EXECUTABLE} - ${GIT_SHA1}")

set(VERSION_SHA1 ${GIT_SHA1})
set(VERSION_DATE ${GIT_DATE})
set(VERSION_SHA1 ${GIT_SHA1} PARENT_SCOPE)
set(VERSION_DATE ${GIT_DATE} PARENT_SCOPE)
endfunction()
