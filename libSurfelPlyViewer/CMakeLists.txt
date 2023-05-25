SET(targetname libSurfelPlyViewer)
SET(sources
        SurfelGUI.cpp
        SurfelDrawer.cpp
#        ImGuiExtension.cpp
        )
SET(headers
        SurfelGUI.h
        SurfelDrawer.h
        )

find_package(Eigen3 REQUIRED)

ADD_LIBRARY(${targetname} ${sources} ${headers})
TARGET_LINK_LIBRARIES(${targetname}
        PUBLIC tinyply
        PUBLIC GUI3D # need to put this the last. otherwise some weird bugs appear. (not able to show text and lines, etc.)
)
target_include_directories(${targetname} PUBLIC ${EIGEN3_INCLUDE_DIR})
TARGET_COMPILE_DEFINITIONS(${targetname} PUBLIC COMPILE_WITH_EIGEN)
