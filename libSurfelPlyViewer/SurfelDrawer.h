//
// Created by sc on 8/21/20.
//
#pragma once
#include <string>
#include <memory>
#include <set>
#include "surfel.h"
#include <GUI3D/glShader.hpp>

namespace PSLAM {
    class SurfelDrawer {
    public:
        SurfelDrawer()=default;
        ~SurfelDrawer();

        void Init(std::function<void(Eigen::Vector3f&, const Surfel*)> getcolor);

        void Update(const std::vector<std::shared_ptr<Surfel>>& surfels);
        void Draw(const Eigen::Matrix4f& projection, const Eigen::Matrix4f& viewMatrix);

        float mDiffuseStength=0.5f;
    private:
        bool bInited=false;
        std::function<void(Eigen::Vector3f&, const Surfel*)> pGetColor = nullptr;
        size_t mPointSize=0, mBufferSize = 1e3;
        unsigned int VAO{},VBO{};
        std::unique_ptr<glUtil::Shader> mShader;

        void UpdateBuffer(size_t newSize, bool force = false);
    };
}
