cmake_minimum_required(VERSION 3.13)
project(SurfelPLYViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function" )
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#################################
# Specify the CMake module path #
#################################
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/libGUI3D/cmake ${CMAKE_MODULE_PATH})

############
# External #
############
#INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseCxxTools.cmake)
#INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseJson.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseTinyPLY.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/libGUI3D/cmake/UseCxxTools.cmake)


######################
# Add subdirectories #
######################


######################
# Add subdirectories #
######################
ADD_SUBDIRECTORY(libGUI3D/libGUI)
ADD_SUBDIRECTORY(libGUI3D/libGUI3D)

ADD_SUBDIRECTORY(libSurfelPlyViewer)

ADD_EXECUTABLE(surfelPlyViewer main.cpp)
TARGET_LINK_LIBRARIES(surfelPlyViewer PRIVATE libSurfelPlyViewer)
