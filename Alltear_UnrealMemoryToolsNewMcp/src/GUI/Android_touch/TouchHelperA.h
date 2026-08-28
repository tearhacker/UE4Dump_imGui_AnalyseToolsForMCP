#pragma once

#include <linux/input.h>
#include <vector>
#include <functional>
#include "VectorStruct.h"

namespace Touch {
    struct touchObj {
        My_Vector2 pos{};
        int id = 0;
        bool isDown = false;
    };

    struct Device {
        int fd;
        float S2TX;
        float S2TY;
        input_absinfo absX, absY;
        touchObj Finger[10];

        Device() { memset((void *) this, 0, sizeof(*this)); }
    };

    bool Init(const My_Vector2 &s, bool p_readOnly);

    void Close();

    void Down(float x, float y);

    void Move(float x, float y);

    void Up();

    void Move(touchObj *touch, float x, float y);

    void Upload();

    void SetCallBack(const std::function<void(std::vector<Device> *)> &cb);

    My_Vector2 Touch2Screen(const My_Vector2 &coord);

    My_Vector2 GetScale();

    void setOrientation(int orientation);

    void setOtherTouch(bool p_otherTouch);
}
