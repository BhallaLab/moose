# Replicate Travis-CI building using docker
add_custom_target(travis
    COMMAND docker build -t bhallalab/travis:latest
        -f ${CMAKE_CURRENT_SOURCE_DIR}/docker/travis/Dockerfile .
    COMMENT "Replicating Travis-CI building using Docker."
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    VERBATIM)

add_custom_target(centos
    COMMAND docker build -t bhallalab/centos:latest
        -f ${CMAKE_CURRENT_SOURCE_DIR}/docker/centos/Dockerfile .
    COMMENT "Building moose on centos"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    VERBATIM)
