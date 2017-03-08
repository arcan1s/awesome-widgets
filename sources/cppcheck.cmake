# additional target to perform cppcheck run, requires cppcheck
set(CPPCHECK_EXECUTABLE "/usr/bin/cppcheck" CACHE STRING "Path to cppcheck executable")

# get all project files
# HACK this workaround is required to avoid qml files checking ^_^
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)
foreach (SOURCE_FILE ${ALL_SOURCE_FILES})
    string(FIND ${SOURCE_FILE} ${PROJECT_TRDPARTY_DIR} PROJECT_TRDPARTY_DIR_FOUND)
    if (NOT ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1)
        list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
    endif ()
endforeach ()

add_custom_target(
        cppcheck
        COMMAND ${CPPCHECK_EXECUTABLE}
        --enable=warning,performance,portability
        --error-exitcode=1
        --std=c++11
        --language=c++
        --library=qt.cfg
        --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
        --verbose
        --quiet
        ${ALL_SOURCE_FILES}
)
