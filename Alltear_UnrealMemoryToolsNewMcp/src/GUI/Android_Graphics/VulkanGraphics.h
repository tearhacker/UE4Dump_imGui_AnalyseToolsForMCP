//
// Created by ITEK on 2024/2/3.
//

#ifndef ANDROIDIMGUI_VULKANGRAPHICS_H
#define ANDROIDIMGUI_VULKANGRAPHICS_H


#include <memory>
#include "vulkan_wrapper.h"
#include "AndroidImgui.h"
#include "imgui_impl_vulkan.h"

class VulkanGraphics : public AndroidImgui {
private:
    struct VulkanTextureData : BaseTexData {
        VkImageView ImageView = VK_NULL_HANDLE;
        VkImage Image = VK_NULL_HANDLE;
        VkDeviceMemory ImageMemory = VK_NULL_HANDLE;
        VkSampler Sampler = VK_NULL_HANDLE;
        VkBuffer UploadBuffer = VK_NULL_HANDLE;
        VkDeviceMemory UploadBufferMemory = VK_NULL_HANDLE;
    };

    VkAllocationCallbacks *m_Allocator = nullptr;
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;
    uint32_t m_QueueFamily = (uint32_t) -1;
    VkQueue m_Queue = VK_NULL_HANDLE;
    VkDebugReportCallbackEXT m_DebugReport = VK_NULL_HANDLE;
    VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

    std::unique_ptr<ImGui_ImplVulkanH_Window> wd{};
    int m_MinImageCount = 2;
    bool m_SwapChainRebuild = false;

    int m_LastWidth = 0;
    int m_LastHeight = 0;
public:
    VulkanGraphics() {
		snprintf(RenderName, sizeof(RenderName), "Vulkan");
    }
    bool Create() override;

    void Setup() override;

    void PrepareFrame(bool resize) override;

    void Render(ImDrawData *drawData) override;

    void PrepareShutdown() override;

    void Cleanup() override;

    BaseTexData *LoadTexture(BaseTexData *tex_data, void *pixel_data) override;

    void RemoveTexture(BaseTexData *tex_data) override;

private:
    VkPhysicalDevice SetupVulkan_SelectPhysicalDevice();

    uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);
};

#endif //ANDROIDIMGUI_VULKANGRAPHICS_H
