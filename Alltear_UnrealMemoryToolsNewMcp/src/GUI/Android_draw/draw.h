#ifndef NATIVESURFACE_DRAW_H
#define NATIVESURFACE_DRAW_H

#include <stdio.h>
#include <stdlib.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "native_surface/ANativeWindowCreator.h"

#include "AndroidImgui.h"
#include "TouchHelperA.h"//触摸
#include "my_imgui.h"     //字体

extern std::unique_ptr<AndroidImgui> graphics;
extern ANativeWindow *window;
extern android::ANativeWindowCreator::DisplayInfo displayInfo;// 屏幕信息
extern ImGuiWindow *g_window;// 窗口信息

extern int abs_ScreenX, abs_ScreenY;// 绝对屏幕X _ Y
extern int native_window_screen_x, native_window_screen_y;
extern TextureInfo Aekun_image;

extern ImFont* zh_font;
extern ImFont* icon_font_0;
extern ImFont* icon_font_1;
extern ImFont* icon_font_2;


// 上次UI位置
struct Last_ImRect {
    float Pos_x;
    float Pos_y;
    float Size_x;
    float Size_y;
};
extern struct Last_ImRect LastCoordinate;
//是否过录制
extern bool permeate_record;
extern bool permeate_record_ini;


extern void screen_config();// 获取屏幕信息
extern void drawBegin();// 布局UI
extern void Layout_tick_UI(bool *main_thread_flag);
extern void init_My_drawdata();// 初始化绘制数据




#endif //NATIVESURFACE_DRAW_H
