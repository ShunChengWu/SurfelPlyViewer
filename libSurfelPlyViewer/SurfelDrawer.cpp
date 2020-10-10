//
// Created by sc on 8/21/20.
//

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
    const std::string fsShader = "#version 330\n"
                                 "in vec3 colorV;\n"
                                 "in vec2 v_surfel_coord;"
                                 "in vec4 normalV;\n"
                                 "out vec4 color;\n"
                                 "out float sq_norm;\n"
                                 "void main() {\n"
                                 "    float sq_norm = dot(v_surfel_coord, v_surfel_coord);\n"
                                 "    if (sq_norm > 1.f) discard;\n"
                                 "    color = vec4(colorV, 1.0);\n"
                                 "}";
    const std::string vsShader = "#version 330\n"
                                 "uniform mat4 model_view_projection;\n"
                                 "uniform mat4 u_projection;\n"
                                 "layout(location = 0) in vec3 position;\n"
                                 "layout(location = 1) in vec3 color;\n"
                                 "layout(location = 2) in vec2 texture;\n"
                                 "layout(location = 3) in vec3 normal;\n"
                                 "out vec2 v_surfel_coord;\n"
                                 "out vec3 colorV;\n"
                                 "out vec4 normalV;\n"
                                 "out vec4 positionV;\n"
                                 "void main() {\n"
                                 "    v_surfel_coord = vec2(2.0 * texture - 1.0)\n;"
                                 "    vec4 normalV = model_view_projection * vec4(normal, 0);"
                                 "    vec3 test = vec3(0.5, 0.5, 1);"
                                 "    float diffuse = abs(dot(normalV.xyz, normalize(test)));" //
                                 "    vec3 color2 = color;"
                                 "    color2 *= diffuse;"
                                 "    vec4 positionV = model_view_projection * vec4(position, 1);"
                                 "    gl_Position = u_projection * positionV;"
                                 "    colorV = color2;\n"
                                 "}";;

    mShader = std::make_unique<glUtil::Shader>(vsShader,fsShader);
    mShader->use();
    UpdateBuffer(mBufferSize,true);
}

//void SurfelDrawer::GetColor(Eigen::Vector3f& surfel_color, const Surfel *surfel) const {
//    //TODO:implement this
//    switch (static_cast<COLORTYPE>(mColorType)) {
//        case COLOR_LABEL:
//        {
//            const cv::Vec3b& color = inseg_lib::CalculateLabelColor(surfel->label);
//            surfel_color << color(2)/255.0f, color(1)/255.0f, color(0)/255.0f;
//        }
//            break;
//        case COLOR_PHONG:
//            surfel_color << 0.9, 0.9, 0.9;
//            break;
//        case COLOR_NORMAL:
//            surfel_color << (surfel->normal.x() + 1.f) / 2.f, (surfel->normal.y() + 1.f) / 2.f, -surfel->normal.z();
//            break;
//        case COLOR_COLOR:
//            surfel_color << surfel->color[2]/255.0f, surfel->color[1]/255.0f, surfel->color[0]/255.0f;
//            break;
//        case COLOR_UPDATED:
//        {
//            const cv::Vec3b& color = inseg_lib::CalculateLabelColor(surfel->label);
//            if (mUpdatedIndices.find(surfel->label) == mUpdatedIndices.end())
//                surfel_color << 0.3f*(color(0)/255.0f), 0.3f*(color(1)/255.0f), 0.3f*(color(2)/255.0f);
//            else
//                surfel_color << color(0)/255.0f, color(1)/255.0f, color(2)/255.0f;
//        }
//            break;
//        case COLOR_SEMANTIC:
////            assert(mpGraph);
////            if(mpGraph->nodes.at(surfel->label)->last_class_predicted>=0){
////                const cv::Vec3b& color = inseg_lib::CalculateLabelColor(mpGraph->nodes.at(surfel->label)->last_class_predicted);
////                surfel_color << color(0)/255.0f, color(1)/255.0f, color(2)/255.0f;
////            } else
////                surfel_color << 0.f,0.f,0.f;
//            break;
//    }
//}

void SurfelDrawer::Update(const std::vector<std::shared_ptr<Surfel>>& surfels){
    VecEigenf(3) points,colors,normals;
    VecEigenf(2) textures;
    points.reserve(surfels.size() * 6);
    colors.reserve(surfels.size() * 6);
    textures.reserve(surfels.size() * 6);
    normals.reserve(surfels.size() * 6);

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
        const float radius = 1.414214f * surfel->radius/1000.0f;

        Eigen::Vector3f u;
        if (std::abs(normal.dot(Eigen::Vector3f::UnitX())) >
            std::abs(normal.dot(Eigen::Vector3f::UnitY()))) {
            u = radius * normal.cross(Eigen::Vector3f::UnitX()).normalized();
        } else {
            u = radius * normal.cross(Eigen::Vector3f::UnitY()).normalized();
        }
        const Eigen::Vector3f v = normal.cross(u);

        Eigen::Vector3f p = point/1000.0f;
        const Eigen::Vector3f p0 = p - u;
        const Eigen::Vector3f p1 = p - v;
        const Eigen::Vector3f p2 = p + u;
        const Eigen::Vector3f p3 = p + v;

        points.emplace_back(p0);
        points.emplace_back(p1);
        points.emplace_back(p2);
        points.emplace_back(p0);
        points.emplace_back(p2);
        points.emplace_back(p3);

        textures.emplace_back(0,0);
        textures.emplace_back(1,0);
        textures.emplace_back(1,1);
        textures.emplace_back(0,0);
        textures.emplace_back(1,1);
        textures.emplace_back(0,1);

        for(size_t s=0;s<6;s++){
            colors.push_back(final_color);
            normals.emplace_back(normal);
        }
        counter++;
    }

    mPointSize = counter*6;
    UpdateBuffer(mPointSize);


    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // position
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    mPointSize * 3 * sizeof(GLfloat), points.data());
    // color
    glBufferSubData(GL_ARRAY_BUFFER, mBufferSize * (3) * sizeof(GLfloat),
                    mPointSize * 3 * sizeof(GLfloat), colors.data());
    // texture
    glBufferSubData(GL_ARRAY_BUFFER, mBufferSize * (3+3) * sizeof(GLfloat),
                    mPointSize * 2 * sizeof(GLfloat), textures.data());
    // normal
    glBufferSubData(GL_ARRAY_BUFFER, mBufferSize * (3+3+2) * sizeof(GLfloat),
                    mPointSize * 3 * sizeof(GLfloat), normals.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SurfelDrawer::Draw(const Eigen::Matrix4f& projection, const Eigen::Matrix4f& viewMatrix) {
    if(!bInited) throw std::runtime_error("NodeDrawer: Draw was called before Init!");
    mShader->use();
    mShader->set("u_projection",projection);
    mShader->set("model_view_projection",viewMatrix);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, mPointSize);
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

    // create buffer
    uint numStride = 3+3+2+3;
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
    // vec2 texture
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                          (void *) (mBufferSize * (3+3) * sizeof(GLfloat)));
    // vec3 normal
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (void *) (mBufferSize * (3+3+2) * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    bInited=true;
}
