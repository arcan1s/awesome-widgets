exec_program(
        "sed -n '1,/^Ver/ p' CHANGELOG | grep -v '^Ver' | tr '\n' '@'"
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        OUTPUT_VARIABLE PROJECT_CHANGELOG
)
