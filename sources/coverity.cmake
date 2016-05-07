# additional target to perform cppcheck run, requires cppcheck
set(COVERITY_COMMENT "" CACHE STRING "Coverity build comment")
set(COVERITY_DIRECTORY "cov-int" CACHE STRING "Path to coverity integration directory")
set(COVERITY_EMAIL "" CACHE STRING "Coverity user email")
set(COVERITY_EXECUTABLE "cov-build" CACHE STRING "Path to coverity build tool")
set(COVERITY_TOKEN "" CACHE STRING "Coverity token")
set(COVERITY_URL "https://scan.coverity.com/builds?project=Awesome+Widgets" CACHE STRING "Coverity project url")

add_custom_target(
        coverity
        COMMAND ${COVERITY_EXECUTABLE}
        --dir ${COVERITY_DIRECTORY}
        make
        COMMAND zip
        -r ${PROJECT_NAME}.zip
        ${COVERITY_DIRECTORY}
)

add_custom_target(
        coverity-upload
        COMMAND curl
        --form token=${COVERITY_TOKEN}
        --form email=${COVERITY_EMAIL}
        --form file=@${PROJECT_NAME}.zip
        --form version=${PROJECT_VERSION}-${PROJECT_COMMIT_SHA}
        --form description=${COVERITY_COMMENT}
        ${COVERITY_URL}
)
