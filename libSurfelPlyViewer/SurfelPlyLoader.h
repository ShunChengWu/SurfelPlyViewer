//
// Created by sc on 10/9/20.
//
#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <tinyply.h>
#include <iostream>
#include "surfel.h"
//#include "MemoryBlock.h"

namespace Util {
    template<typename T>
    static void copyFromBuffer(tinyply::PlyData *buffer, std::vector<T> *container) {
        if (std::is_same<T, int8_t>::value) assert(buffer->t == tinyply::Type::INT8);
        if (std::is_same<T, uint8_t>::value) assert(buffer->t == tinyply::Type::UINT8);
        if (std::is_same<T, int16_t>::value) assert(buffer->t == tinyply::Type::INT16);
        if (std::is_same<T, uint16_t>::value) assert(buffer->t == tinyply::Type::UINT16);
        if (std::is_same<T, int32_t>::value) assert(buffer->t == tinyply::Type::INT32);
        if (std::is_same<T, uint32_t>::value) assert(buffer->t == tinyply::Type::UINT32);
        if (std::is_same<T, float>::value) {
            assert(buffer->t == tinyply::Type::FLOAT32);
        }
        if (std::is_same<T, double>::value) assert(buffer->t == tinyply::Type::FLOAT64);
//    std::cout << "buffer->count: " << buffer->count << "\n";
//    std::cout << "buffer->buffer.size_bytes(): " << buffer->buffer.size_bytes() << "\n";
//    std::cout << buffer->buffer.size_bytes()/buffer->count << "\n";
//    std::cout << tinyply::PropertyTable.at(buffer->t).str << "\n";
        container->resize(buffer->count);
        const size_t numVerticesBytes = buffer->buffer.size_bytes();
        std::memcpy(container->data(), buffer->buffer.get(), numVerticesBytes);
//        std::memcpy(container->data(), buffer->buffer.get(), sizeof(T) * buffer->count);
    }
    void print_headers(const tinyply::PlyFile &file){
        for (const auto &e : file.get_elements()) {
            std::cout << "\t[ply_header] element: " << e.name << " (" << e.size << ")" << std::endl;
            for (const auto &p : e.properties) {
                std::cout << "\t[ply_header] \tproperty: " << p.name << " (type="
                          << tinyply::PropertyTable[p.propertyType].str << ")";
                if (p.isList) std::cout << " (list_type=" << tinyply::PropertyTable[p.listType].str << ")";
                std::cout << std::endl;
            }
        }
    }

    static std::vector<std::shared_ptr<Surfel>> load_cloud(const std::string &path) {
//    auto dim_pts = 3;
        std::unique_ptr <std::istream> file_stream;
        // try to load with binary
        std::vector <uint8_t> byte_buffer;

        // try to load with binary first
        file_stream = std::make_unique<std::ifstream>(path, std::ios::binary);
        if (!file_stream || file_stream->fail()) throw std::runtime_error("file_stream failed to open " + path);
        tinyply::PlyFile file;
        file.parse_header(*file_stream);

        print_headers(file);

        std::shared_ptr <tinyply::PlyData> point_data, label_data, normal_data, rgb_data, /*quality_data,*/ radius_data;

        bool bGoodRadius=true;
        try {
            std::vector <std::string> propertyKeys = {"x", "y", "z"};
            point_data = file.request_properties_from_element("vertex", propertyKeys);
        } catch (const std::exception &e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try {
            label_data = file.request_properties_from_element("vertex", {"label"});
        } catch (const std::exception &e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try {
            std::vector <std::string> propertyKeys = {"nx", "ny", "nz"};
            normal_data = file.request_properties_from_element("vertex", propertyKeys);
        } catch (const std::exception &e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try {
            std::vector <std::string> propertyKeys = {"red", "green", "blue"};
            rgb_data = file.request_properties_from_element("vertex", propertyKeys);
        } catch (const std::exception &e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

//        try {
//            quality_data = file.request_properties_from_element("vertex", {"quality"});
//        } catch (const std::exception &e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try {
            radius_data = file.request_properties_from_element("vertex", {"radius"});
        } catch (const std::exception &e) {
            bGoodRadius = false;
            std::cerr << "tinyply exception: " << e.what() << std::endl; }
//        bGoodRadius = false;

        file.read(*file_stream);

        struct float3{
            float x,y,z;
        };
        std::vector<float3> points;
        if (!point_data) return {};
        copyFromBuffer(point_data.get(), &points);

        std::vector<ushort> labels;
        copyFromBuffer(label_data.get(), &labels);

        std::vector<float3> normals;
        copyFromBuffer(normal_data.get(), &normals);

        struct uchar3{
            unsigned char x,y,z;
        };
        std::vector<uchar3> colors;
        copyFromBuffer(rgb_data.get(), &colors);

        std::vector<float> /*qulity, */radius;
//        copyFromBuffer(quality_data.get(), &qulity);
        if(bGoodRadius)copyFromBuffer(radius_data.get(), &radius);

        std::vector<std::shared_ptr<Surfel>> surfels(points.size());
        Eigen::Vector3f centroid = {0.f,0.f,0.f};
        for(size_t i=0;i<points.size();++i ){
            surfels[i].reset(new Surfel());
            auto& surfel = surfels[i];
            if (std::isnan(points[i].x) || std::isnan(points[i].y) || std::isnan(points[i].z) ||
            std::isinf(points[i].x) || std::isinf(points[i].y) || std::isinf(points[i].z)) {
                printf("has nan\n");
                surfel->pos.setZero();
                surfel->is_valid = false;
                surfel->is_stable = false;
            } else {
                surfel->pos.x() = points[i].x;
                surfel->pos.y() = points[i].y;
                surfel->pos.z() = points[i].z;
                centroid += surfel->pos;
            }

            if(colors.size() == points.size()){
                surfel->color[2] = colors[i].x;
                surfel->color[1] = colors[i].y;
                surfel->color[0] = colors[i].z;
            }
            if(normals.size() == points.size()) {
                surfel->normal.x() = normals[i].x;
                surfel->normal.y() = normals[i].y;
                surfel->normal.z() = normals[i].z;
            }
            if(labels.size() == points.size()){
                surfel->label = labels[i];
            }
//            if(qulity.size() == points.size()){
                surfel->label_confidence = 100/*qulity[i]*/;
//            }
            if(radius.size() == points.size()) {
                surfel->radius = radius[i];
            } else
                surfel->radius = 10.f;
            surfel->is_valid = true;
            surfel->is_stable = true;
        }
        std::cout << centroid.transpose() << "\n";
        centroid /= points.size();
        for(size_t i=0;i<points.size();++i )
            surfels[i]->pos -= centroid;

        return surfels;
    }
}