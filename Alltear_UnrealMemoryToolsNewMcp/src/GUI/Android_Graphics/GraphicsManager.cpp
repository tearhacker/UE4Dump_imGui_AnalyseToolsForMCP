//
// Created by ITEK on 2024/2/3.
//

#include "GraphicsManager.h"
#include "VulkanGraphics.h"
#include "OpenGLGraphics.h"


std::unique_ptr<AndroidImgui> GraphicsManager::getGraphicsInterface(GraphicsAPI api) {
    switch (api) {
        case OPENGL:{
            std::unique_ptr<AndroidImgui> at = std::make_unique<OpenGLGraphics>();
            return at;
        };
        case VULKAN:{
            std::unique_ptr<AndroidImgui> at = std::make_unique<VulkanGraphics>();
            return at;
        };
    }
    return nullptr;
}
