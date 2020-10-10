###

include(${PROJECT_SOURCE_DIR}/cmake/cmake_macro_addExternalProject/macro.cmake)

addExternalProjectGit (
        tinyply
        git@github.com:ddiakopoulos/tinyply.git
        tinyply_DIR
)
#message("tinyply_DIR: " ${tinyply_DIR})
include(${tinyply_DIR}/lib/cmake/tinyply/tinyplyConfig.cmake)
link_directories(${tinyply_DIR}/lib)