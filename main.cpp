#include "libSurfelPlyViewer/SurfelGUI.h"
#include <string>
int main(int argc, char** argv){
    std::string path="./";
    if(argc>1) path = argv[1];
    PSLAM::SurfelGUI gui(path);
    gui.run();
}
