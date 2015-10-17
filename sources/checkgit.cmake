exec_program(
        "git"
        ${CMAKE_CURRENT_SOURCE_DIR}
        ARGS "log" "-1" "--format=\"%h\""
        OUTPUT_VARIABLE COMMIT_SHA
        RETURN_VALUE GIT_RETURN
)

if (${GIT_RETURN} EQUAL "0")
    set(PROJECT_COMMIT_SHA "${COMMIT_SHA}")
endif ()
