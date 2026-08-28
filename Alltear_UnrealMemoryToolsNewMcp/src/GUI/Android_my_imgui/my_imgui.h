#pragma once

#include "imgui.h"

namespace ImGui {    
    extern ImFont* SystemFont;
    

    ImFont* My_AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg_template, const ImWchar* glyph_ranges);

    const ImWchar *GetGlyphRangesChineseTraditionalOfficial();

    const ImWchar *GetGlyphRangesChineseSimplifiedOfficial();

    IMGUI_API bool              My_Android_LoadSystemFont(float SizePixels);
    IMGUI_API ImFont*           My_AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);
} 