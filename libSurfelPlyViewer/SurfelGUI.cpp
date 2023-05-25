//
// Created by sc on 8/21/20.
//

#include "SurfelGUI.h"
#include <ImGuiExtension/LocalFileSystem.h>
//#include "ImGuiExtension.h"
using namespace PSLAM;

SurfelGUI::SurfelGUI(const std::string &path)
        : SC::GUI3D("Surfel Ply Viewer", 1280, 800)
{
    registerKeyFunciton(window_, GLFW_KEY_ESCAPE,
                        [&]() {
                            glfwSetWindowShouldClose(window_->window, true);
                        });

    if (!path.empty()) selected_path = path;
    selected_path.copy(text_buffer,selected_path.length());
    mSurfelDrawer.Init(std::bind(&SurfelGUI::GetSurfelColor,this,std::placeholders::_1,std::placeholders::_2));
    generateList(labelColorList, 5096);

    glCam.reset(new glUtil::Camera(window_->width,window_->height,camPose, camUp, yaw, pitch));
    glCam->camera_control_.reset(new SC::ArcCameraContorl());
}

void SurfelGUI::drawUI() {
//    SC::GUI3D::drawUI();
    MainUI();
    cameraUI();
    glCam->drawUI();
    mouseControl();
}

void SurfelGUI::drawGL() {
    processInput(window_->window);
    basicProcess();
    Process();
}

void SurfelGUI::scale_cloud() {
    for(auto &surfel:surfels){
        surfel->pos *= scale;
    }
}

void SurfelGUI::scale_radius() {
    for(auto &surfel:surfels){
        surfel->radius *= scale;
    }
}

void SurfelGUI::MainUI(){
    if(!ImGui::Begin("Control Panel",nullptr, ImGuiWindowFlags_MenuBar)){
        ImGui::End();
        return;
    }
//    ImGui::Text("Selected Path");
//    ImGui::Text("%s", selected_path.c_str());
    if(ImGui::InputText("path",text_buffer,IM_ARRAYSIZE(text_buffer))) selected_path = text_buffer;

//    constexpr size_t kFilenameBufSize = 512;
//    static char name_buf[kFilenameBufSize];
    if (ImGui::FileSelectButton("select_file",text_buffer,buffer_size)) {
        selected_path = text_buffer;
//        printf("%s\n",name_buf);
//        selected_path = std::string(name_buf);
    }

//    if (ImGui::Button("Open File Dialog"))
//        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".ply\0\0", selected_path,0);
//    if (ImGuiFileDialog::Instance()->FileDialog("ChooseFileDlgKey"))
//    {
//        // action if OK
//        if (ImGuiFileDialog::Instance()->IsOk)
//        {
//            std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
//            std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
//            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
//            auto selections = ImGuiFileDialog::Instance()->GetSelection();
//            // action
////            printf("Selected file:\n");
////            printf("%s\n%s\n%s\n", fileName.c_str(), filePathName.c_str(), filePath.c_str());
////            printf("Selected files:\n");
//            for(auto v : selections) {
//                selected_path = v.second;
//                break;
//            }
//            selected_path.copy(text_buffer,selected_path.length());
//            printf("select path: %s\n", selected_path.c_str());
//        }
//        // close
//        ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");
//    }


    if(ImGui::Button("Load")){
        LoadCloud();
        bNeedUpdate=true;
    }

    ImGui::Checkbox("face culling",&face_culling);

    if(ImGui::InputFloat("scale value",&scale)){

    }
    ImGui::DragFloat("diffuse strength",&mSurfelDrawer.mDiffuseStength,0.01,0.f,1.f);


    if(ImGui::Button("point scale") ) {
        printf("scale pts by %f\n", scale);
        scale_cloud();
        bNeedUpdate=true;
    }
    if(ImGui::Button("radius scale") ) {
        printf("scale radius by %f\n", scale);
        scale_radius();
        bNeedUpdate=true;
    }

    /// Color Rendering Menu
    {
        static int selected_color_type = mColorType;
        const char* color_names[] = {"LABEL", "PHONG", "NORMAL", "COLOR"};
//        if (ImGui::CollapsingHeader("Rendering")){
        ImGui::Text("Color Render Type"); ImGui::SameLine();
        if (ImGui::Button(color_names[selected_color_type]))
            ImGui::OpenPopup("surfel_rendering_popup");
        ImGui::SameLine();
        if (ImGui::BeginPopup("surfel_rendering_popup"))
        {
            for (int i = 0; i < IM_ARRAYSIZE(color_names); i++)
                if (ImGui::Selectable(color_names[i]))
                    selected_color_type = i;
            ImGui::EndPopup();
        }
        if(mColorType != selected_color_type){
            mColorType = selected_color_type;
            bNeedUpdate=true;
        }
        ImGui::NewLine();
    }

    ImGui::End();
}
#include "SurfelPlyLoader.h"
void SurfelGUI::LoadCloud(){
    try {
        surfels = Util::load_cloud(selected_path);
    } catch (...) {
        std::cerr << "Loading cloud failed. " << selected_path;
    }
    std::cerr << "there are " << surfels.size() << " surfels loaded.\n";
}


void SurfelGUI::Process(){
    const Eigen::Matrix4f eigen_proj = GLM2E<float,4,4>(glCam->projection_control_->projection_matrix());
    const Eigen::Matrix4f eigen_vm   = GLM2E<float,4,4>(glCam->camera_control_->GetViewMatrix());

    auto io  = ImGui::GetIO();
    size_t windowWidth = io.DisplaySize.x, windowHeight = io.DisplaySize.y;
#ifdef APPLE
    windowWidth *=2;
    windowHeight*=2;
#endif
    /// Draw3D
    if(face_culling) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
    glEnable(GL_DEPTH_TEST);
    if(bRenderSurfel) {
        if(bNeedUpdate) {
            mSurfelDrawer.Update(surfels);
            bNeedUpdate = false;
        }
        mSurfelDrawer.Draw(eigen_proj,eigen_vm);
    }
//    Eigen::Matrix4f pose = mpGraphSLAM->GetInSeg()->pose().inverse();
//    pose.topRightCorner<3,1>() /= 1000.f;
}

void SurfelGUI::generateList(std::vector<color> &labelColorList, size_t size) {
    labelColorList.resize(size);
    std::random_device rand;
    std::default_random_engine e1(rand());
//    std::uniform_int_distribution<int> dist(0, 255);
    std::uniform_real_distribution<float> dist (0,1);
    labelColorList.at(0) = {1,1,1};
    for (size_t i = 1; i < size; i++) {
        labelColorList.at(i) = {dist(e1),dist(e1),dist(e1)};
    }
}
