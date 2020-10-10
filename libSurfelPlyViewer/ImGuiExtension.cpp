//
// Created by sc on 8/19/20.
//

#include "ImGuiExtension.h"

using namespace ImGui;

static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
    const char* const* items = (const char* const*)data;
    if (out_text)
        *out_text = items[idx];
    return true;
}

//bool ImGui::FilterListBox(ImGuiTextFilter *filter,const char* label, int* current_item, const std::vector<std::string> &items, int items_count, int height_items)
//{
//    const bool value_changed = FilterListBox(filter, label, current_item, items, items_count, height_items);
//    return value_changed;
//}

bool ImGui::FilterListBox(ImGuiTextFilter *filter,const char* label, int* current_item, const std::vector<std::string> &items, int height_in_items)
{
    filter->Draw(label);
    if (!ListBoxHeader("", items.size(), height_in_items))
        return false;
//    static ImGuiTextFilter filter;
    // Assume all items have even height (= 1 line of text). If you need items of different or variable sizes you can create a custom version of ListBox() in your code without using the clipper.
    ImGuiContext& g = *GImGui;
    bool value_changed = false;
    ImGuiListClipper clipper(items.size(), GetTextLineHeightWithSpacing()); // We know exactly our line height here so we pass it as a minor optimization, but generally you don't need to.
    while (clipper.Step())
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            const bool item_selected = (i == *current_item);
            const std::string* item_text;
            try {
                item_text = &items.at(i);
            } catch (...) {
                item_text = nullptr;
            }

            PushID(i);
            if (filter->PassFilter(item_text->c_str()))
            {
                if (Selectable(item_text->c_str(), item_selected))
                {
                    *current_item = i;
                    value_changed = true;
                }
            }
//            if (Selectable(item_text, item_selected))
//            {
//                *current_item = i;
//                value_changed = true;
//            }
            if (item_selected) {
                SetItemDefaultFocus();
                memcpy(filter->InputBuf,item_text->c_str(),256);
            }
            PopID();
        }
    ListBoxFooter();
    if (value_changed)
        MarkItemEdited(g.CurrentWindow->DC.LastItemId);
    return value_changed;
}