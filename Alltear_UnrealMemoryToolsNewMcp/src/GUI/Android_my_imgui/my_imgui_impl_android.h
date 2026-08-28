#pragma once

#include "imgui.h"

struct ANativeWindow;
struct AInputEvent;

bool My_ImGui_ImplAndroid_Init(ANativeWindow *window);

int32_t My_ImGui_ImplAndroid_HandleInputEvent(AInputEvent *input_event);

int32_t My_ImGui_ImplAndroid_HandleInputEvent_old(AInputEvent *input_event);

void My_ImGui_ImplAndroid_Shutdown();

void My_ImGui_ImplAndroid_NewFrame(bool resize = false);