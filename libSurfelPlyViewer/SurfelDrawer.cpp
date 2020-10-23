//
// Created by sc on 8/21/20.
//

//#include <inseg_lib/map.h>
#include "SurfelDrawer.h"
#include "ImGuiExtension.h"
#include <Eigen/Core>
#define VecEigenf(x) std::vector<Eigen::Matrix<float,x,1>,Eigen::aligned_allocator<Eigen::Matrix<float,x,1>>>

using namespace PSLAM;

SurfelDrawer::~SurfelDrawer(){
    if(bInited) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

void SurfelDrawer::Init(std::function<void(Eigen::Vector3f&, const Surfel*)> getcolor){
    pGetColor = getcolor;
    std::string shader_folder =std::string( GUI_FOLDER_PATH) + "/Shaders/";
    const std::string vsShader = shader_folder+"/surfel.vs";
    const std::string fsShader = shader_folder+"/surfel.fs";
    const std::string gsShader = shader_folder+"/surfel.gs";

    mShader = std::make_unique<glUtil::Shader>(vsShader,fsShader,gsShader);
    mShader->use();
    mShader->set("model", glm::mat4(1.f));
    mShader->set("diffuse_strength",mDiffuseStength);
    UpdateBuffer(mBufferSize,true);
}

void SurfelDrawer::Update(const std::vector<std::shared_ptr<Surfel>>& surfels){
    VecEigenf(3) points,colors,normals;
    VecEigenf(1) radius;
    points.reserve(surfels.size() );
    colors.reserve(surfels.size() );
    radius.reserve(surfels.size());
    normals.reserve(surfels.size());

    Eigen::Vector3f final_color = {0.f,0.f,0.f};

    size_t counter=0;
    for(const auto & i : surfels){
        auto * surfel = i.get();
        if ((!surfel->is_valid || !surfel->is_stable) || (surfel->label == 0)) {
            continue;
        }
        assert(surfel->label!=0);
        const Eigen::Vector3f& point = surfel->pos;
        const Eigen::Vector3f& normal = surfel->normal;
        pGetColor(final_color,surfel);
        points.emplace_back(point*1e-3);
        normals.emplace_back(normal);
        colors.emplace_back(final_color);
        radius.emplace_back(surfel->radius*1e-3);
        counter++;
//        std::cerr << "radius" << surfel->radius*1e-3;
//        std::cerr << "points" << point.transpose()*1e-3;
//        break;
    }

    mPointSize = counter;
    UpdateBuffer(mPointSize);


    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // position
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    mPointSize * 3 * sizeof(GLfloat), points.data());
    // color
    glBufferSubData(GL_ARRAY_BUFFER, mBufferSize * (3) * sizeof(GLfloat),
                    mPointSize * 3 * sizeof(GLfloat), colors.data());
//    // texture
//    glBufferSubData(GL_ARRAY_BUFFER, mBufferSize * (3+3) * sizeof(GLfloat),
//                    mPointSize * 2 * sizeof(GLfloat), textures.data());
    // normal
    glBufferSubData(GL_ARRAY_BUFFER, mBufferSize * (3+3) * sizeof(GLfloat),
                    mPointSize * 3 * sizeof(GLfloat), normals.data());

    glBufferSubData(GL_ARRAY_BUFFER, mBufferSize * (3+3+3) * sizeof(GLfloat),
                    mPointSize * 1 * sizeof(GLfloat), radius.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SurfelDrawer::Draw(const Eigen::Matrix4f& projection, const Eigen::Matrix4f& viewMatrix) {
    if(!bInited) throw std::runtime_error("NodeDrawer: Draw was called before Init!");
    mShader->use();
    mShader->set("diffuse_strength",mDiffuseStength);
    mShader->set("projection", projection);
    mShader->set("view", viewMatrix);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, mPointSize);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SurfelDrawer::UpdateBuffer(size_t newSize, bool force){
    if(bInited) {
        if (!force) {
            if (mBufferSize > newSize) return;
        } else {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
        }
    }
    mBufferSize = (std::floor(newSize / 1e6) + 1) * 1e6;
//    SCLOG(DEBUG) << "new surfel buffer size: " << mBufferSize;

    // create buffer
    uint numStride = 3+3+3+1;
    size_t totalSize = mBufferSize * numStride;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * totalSize, nullptr, GL_DYNAMIC_DRAW);

    // vec3 Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    // vec3 Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (void *) (mBufferSize * (3) * sizeof(GLfloat)));
//    // vec2 texture
//    glEnableVertexAttribArray(2);
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
//                          (void *) (mBufferSize * (3+3) * sizeof(GLfloat)));
    // vec3 normal
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (void *) (mBufferSize * (3+3) * sizeof(GLfloat)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 1 * sizeof(GLfloat),
                          (void *) (mBufferSize * (3+3+3) * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    bInited=true;
}