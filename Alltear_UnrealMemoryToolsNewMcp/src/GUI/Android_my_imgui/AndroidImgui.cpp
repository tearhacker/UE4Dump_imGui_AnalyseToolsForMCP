#include <android/native_window.h>
#include "AndroidImgui.h"
#include "imgui.h"
#include "my_imgui_impl_android.h"
#include "stb_image.h"

bool AndroidImgui::Init_Render(ANativeWindow *window, float width, float height) {
    m_Window = window;
    m_Width = width;
    m_Height = height;

    ANativeWindow_acquire(window);
    Create();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.DisplaySize = {width, height};
    //io.FontGlobalScale = 1.3f;
    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight(); //白
    ImGuiStyle &style = ImGui::GetStyle();
    //style.ScaleAllSizes(1.5);
    //style.WindowRounding = 3.f;
    My_ImGui_ImplAndroid_Init(window);

    Setup();
    return true;
}

void AndroidImgui::NewFrame(bool resize) {
    PrepareFrame(resize);
    My_ImGui_ImplAndroid_NewFrame(resize);
    ImGui::NewFrame();
}

void AndroidImgui::EndFrame() {
    ImGui::Render();
    Render(ImGui::GetDrawData());
}

void AndroidImgui::Shutdown() {
    for (auto &texture: m_Textures) {
        RemoveTexture(texture);
    }
    m_Textures.clear();
    PrepareShutdown();
    My_ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
    Cleanup();
    ANativeWindow_release(m_Window);
}

TextureInfo AndroidImgui::LoadTextureData(const std::function<unsigned char *(BaseTexData *)> &loadFunc) {
    TextureInfo ret_data{0};
    BaseTexData tex_data{};

    tex_data.Channels = 4;
    unsigned char *image_data = loadFunc(&tex_data);
    if (image_data == nullptr)
        return ret_data;

    auto result = LoadTexture(&tex_data, image_data);

    stbi_image_free(image_data);

    if (result != NULL) {
        m_Textures.push_back(result);
    }
    ret_data.DS = (unsigned long long)result->DS;
    ret_data.w = result->Width;
    ret_data.h = result->Height;
    //delete(result);
    return ret_data;
}

TextureInfo AndroidImgui::LoadTextureFromFile(const char *filepath) {
    return LoadTextureData([filepath](BaseTexData *tex_data) {
        return stbi_load(filepath, &tex_data->Width, &tex_data->Height, nullptr, tex_data->Channels);
    });
}

TextureInfo AndroidImgui::LoadTextureFromMemory(void *data, int len) {
    return LoadTextureData([data, len](BaseTexData *tex_data) {
        return stbi_load_from_memory((const stbi_uc *) data, len, &tex_data->Width, &tex_data->Height, nullptr, tex_data->Channels);
    });
}

TextureInfo_gif AndroidImgui::LoadTextureFromMemory_gif(void *data, int len) {
    int w, h, n;
    int *delays = NULL, frames = 0;
    TextureInfo_gif textureInfo_gif{0};

    stbi_uc *gif_alldata = stbi_load_gif_from_memory((const stbi_uc *) data, len, &delays, &w, &h, &frames, &n, 0);
    if (gif_alldata == NULL) {
        return textureInfo_gif;
    }
    textureInfo_gif.DS = (ImTextureID *)malloc(sizeof(ImTextureID) * frames);
    for (int vi = 0; vi < frames; vi++) {
        BaseTexData tex_data{};
        tex_data.Width = w;
        tex_data.Height = h;
        tex_data.Channels = 4;
        stbi_uc *image_data = (stbi_uc *)(gif_alldata + (vi * w * h * n));
        auto result = LoadTexture(&tex_data, image_data);
        if (result != NULL) {
            m_Textures.push_back(result);
        }
        textureInfo_gif.DS[vi] = (unsigned long long)result->DS;   
    }
    stbi_image_free((void *)gif_alldata);        
    textureInfo_gif.w = w;
    textureInfo_gif.h = h;
    textureInfo_gif.frames = frames;
    textureInfo_gif.delays = delays;
    return textureInfo_gif;
}




void AndroidImgui::DeleteTexture(BaseTexData *tex_data) {
    RemoveTexture(tex_data);
    auto it = std::find(m_Textures.begin(), m_Textures.end(), tex_data);
    if (it != m_Textures.end()) {
        m_Textures.erase(it);
    }
}
