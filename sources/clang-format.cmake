# additional target to perform clang-format run, requires clang-format
set(CLANGFORMAT_EXECUTABLE "/usr/bin/clang-format" CACHE STRING "Path to clang-format executable")

# get all project files
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h *.h.in)
foreach (SOURCE_FILE ${ALL_SOURCE_FILES})
    string(FIND ${SOURCE_FILE} ${PROJECT_TRDPARTY_DIR} PROJECT_TRDPARTY_DIR_FOUND)
    if (NOT ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1)
        list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
    endif ()
endforeach ()

add_custom_target(
        clangformat
        COMMAND ${CLANGFORMAT_EXECUTABLE}
        -style=file
        -i
        ${ALL_SOURCE_FILES}
)
