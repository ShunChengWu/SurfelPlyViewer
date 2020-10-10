//
// Created by sc on 8/21/20.
//

#ifndef GRAPHSLAM_SURFELGUI_H
#define GRAPHSLAM_SURFELGUI_H
#include "surfel.h"
#include "../libGUI3D/libGUI3D/GUI3D.h"
#include "eigen_glm.h"
#include "SurfelDrawer.h"
#include <random>

namespace PSLAM {

    class SurfelGUI : public SC::GUI3D {
    public:
        enum COLORTYPE {
            COLOR_LABEL,COLOR_PHONG,COLOR_NORMAL,COLOR_COLOR
        };
        SurfelGUI(const std::string &path="");

        void drawUI() override;

        void drawGL() override;

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    private:
        struct color {
            float r,g,b;
        };

        std::vector<color> labelColorList;
        std::vector<std::shared_ptr<Surfel>> surfels;
        std::string selected_path="./";
        int mColorType = COLOR_LABEL;
        int mItemCurrent = 0;
        SurfelDrawer mSurfelDrawer;
        bool bRenderSurfel=true;
        bool bNeedUpdate=false;
        bool face_culling = true;
        float scale = 1.f;

        void key_callback_impl(GLFWwindow* window, int key, int scancode, int action, int mods)override {}
        void LoadCloud();
        void MainUI();
        void Process();

        void generateList(std::vector<color> &labelColorList, size_t size);
        void scale_cloud();

        void GetSurfelColor(Eigen::Vector3f& surfel_color, const Surfel *surfel) const{
            switch (mColorType) {
                case COLOR_LABEL: {
                    surfel_color << labelColorList[surfel->label].r, labelColorList[surfel->label].g, labelColorList[surfel->label].b;
                }
                    break;
                case COLOR_PHONG:
                    surfel_color << 0.9, 0.9, 0.9;
                    break;
                case COLOR_NORMAL:
                    surfel_color << (surfel->normal.x() + 1.f) / 2.f, (surfel->normal.y() + 1.f) /
                                                                      2.f, -surfel->normal.z();
                    break;
                case COLOR_COLOR:
                    surfel_color << surfel->color[2] / 255.0f, surfel->color[1] / 255.0f, surfel->color[0] / 255.0f;
                    break;
            }
        };
    };
}

#endif //GRAPHSLAM_SURFELGUI_H
