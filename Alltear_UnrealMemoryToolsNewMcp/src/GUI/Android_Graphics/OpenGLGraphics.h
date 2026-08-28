//
// Created by ITEK on 2024/2/3.
//

#ifndef ANDROIDIMGUI_OPENGLGRAPHICS_H
#define ANDROIDIMGUI_OPENGLGRAPHICS_H

#include <EGL/egl.h>
#include "AndroidImgui.h"

class OpenGLGraphics : public AndroidImgui {
private:
    struct OpenglTextureData : BaseTexData {
    };

    EGLDisplay m_EglDisplay = EGL_NO_DISPLAY;
    EGLSurface m_EglSurface = EGL_NO_SURFACE;
    EGLContext m_EglContext = EGL_NO_CONTEXT;
public:
    OpenGLGraphics() {
		snprintf(RenderName, sizeof(RenderName), "OpenGL");
    }
    bool Create() override;

    void Setup() override;

    void PrepareFrame(bool resize) override;

    void Render(ImDrawData *drawData) override;

    void PrepareShutdown() override;

    void Cleanup() override;

    BaseTexData *LoadTexture(BaseTexData *tex_data, void *pixel_data) override;

    void RemoveTexture(BaseTexData *tex_data) override;
};


#endif //ANDROIDIMGUI_OPENGLGRAPHICS_H
