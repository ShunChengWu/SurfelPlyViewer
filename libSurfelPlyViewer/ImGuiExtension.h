//
// Created by sc on 8/19/20.
//
#ifndef GT_GRAPH_VIEWER_IMGUIEXTENSION_H
#define GT_GRAPH_VIEWER_IMGUIEXTENSION_H
#include "imgui.h"
#include "imgui_internal.h"
#include <string>
#include <vector>
namespace ImGui {
//    IMGUI_API bool          FilterListBox(ImGuiTextFilter *filter, const char* label, int* current_item, const char* const items[], int items_count, int height_in_items = -1);
//    IMGUI_API bool          FilterListBox(ImGuiTextFilter *filter, const char* label, int* current_item, bool (*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1);

    IMGUI_API bool          FilterListBox(ImGuiTextFilter *filter, const char* label, int* current_item, const std::vector<std::string> &items, int height_in_items = -1);
}

#endif //GT_GRAPH_VIEWER_IMGUIEXTENSION_H
