#The name of the project
PROJECT(DeSTIN_CUDA_Alt)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# uncomment to be in Release mode
# SET(CMAKE_BUILD_TYPE Release)

# uncomment to build in debug mode
# SET(CMAKE_BUILD_TYPE Debug)

IF (CMAKE_BUILD_TYPE STREQUAL "")
	SET(CMAKE_BUILD_TYPE Release)
ENDIF()

IF(CMAKE_BUILD_TYPE STREQUAL "Debug" )
    SET(dbg_flags -g )
ENDIF()

INCLUDE_DIRECTORIES( include )


ADD_EXECUTABLE(destin src/destin_mnist.c )    

ADD_LIBRARY( destinalt SHARED src/destin.c src/node.c src/util.c )


TARGET_LINK_LIBRARIES( destin destinalt cxcore cv highgui )
SET_TARGET_PROPERTIES( destin PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin )

