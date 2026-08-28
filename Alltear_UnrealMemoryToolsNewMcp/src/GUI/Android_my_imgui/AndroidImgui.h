#ifndef ANDROIDIMGUI_ANDROIDIMGUI_H
#define ANDROIDIMGUI_ANDROIDIMGUI_H

#include <memory>
#include <functional>
#include <vector>


struct ANativeWindow;
struct ImDrawData;

struct BaseTexData {
    void *DS = nullptr;
    int Width = 0;
    int Height = 0;
    int Channels = 0;

    BaseTexData() = default;

    BaseTexData(BaseTexData &other) = default;
};

struct TextureInfo {
    unsigned long long DS = 0;
    int w;
    int h;
};

struct TextureInfo_gif {
    unsigned long long *DS = nullptr;
    int *delays;
    int frames;
    int w;
    int h;
};

class AndroidImgui {
protected:
    ANativeWindow *m_Window;
    float m_Width;
    float m_Height;

    std::vector<BaseTexData *> m_Textures;
public:
    char RenderName[16];

public:
    AndroidImgui() = default;

    virtual ~AndroidImgui() = default;

    bool Init_Render(ANativeWindow *window, float width, float height);

    void NewFrame(bool resize = false);

    void EndFrame();

    void Shutdown();

    TextureInfo LoadTextureFromFile(const char *filepath);

    TextureInfo LoadTextureFromMemory(void *data, int len);
    TextureInfo_gif LoadTextureFromMemory_gif(void *data, int len);

    void DeleteTexture(BaseTexData *tex_data);
    
private:
    TextureInfo LoadTextureData(const std::function<unsigned char *(BaseTexData *)> &loadFunc);

    virtual bool Create() = 0;

    virtual void Setup() = 0;

    virtual void PrepareFrame(bool resize) = 0;

    virtual void Render(ImDrawData *drawData) = 0;

    virtual void PrepareShutdown() = 0;

    virtual void Cleanup() = 0;

    virtual BaseTexData *LoadTexture(BaseTexData *tex_data, void *pixel_data) = 0;

    virtual void RemoveTexture(BaseTexData *tex_data) = 0;
};


#endif //ANDROIDIMGUI_ANDROIDIMGUI_H
