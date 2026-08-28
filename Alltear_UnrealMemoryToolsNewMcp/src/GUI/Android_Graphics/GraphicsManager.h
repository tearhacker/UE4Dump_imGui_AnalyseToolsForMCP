//
// Created by ITEK on 2024/2/3.
//

#ifndef ANDROIDIMGUI_GRAPHICSMANAGER_H
#define ANDROIDIMGUI_GRAPHICSMANAGER_H

#include "AndroidImgui.h"
#include <memory>


class GraphicsManager {
public:
    enum GraphicsAPI {
        OPENGL,
        VULKAN
    };

    static std::unique_ptr<AndroidImgui> getGraphicsInterface(GraphicsAPI api);
};


#endif //ANDROIDIMGUI_GRAPHICSMANAGER_H
