execute_process(
        COMMAND "sed -n '1,/^Ver/ p' CHANGELOG 2> /dev/null | grep -v '^Ver' | tr '\n' '@'"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE PROJECT_CHANGELOG
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
