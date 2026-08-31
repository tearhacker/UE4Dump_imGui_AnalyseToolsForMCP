/*
 * MIT License
 *
 * Copyright (c) 2023 AFan4724
 * Project: https://github.com/AFan4724/AndroidSurfaceImgui-Enhanced
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef A_NATIVE_WINDOW_CREATOR_H // !A_NATIVE_WINDOW_CREATOR_H
#define A_NATIVE_WINDOW_CREATOR_H

#include <android/native_window.h>
#include <android/log.h>
#include <dlfcn.h>
#include <sys/system_properties.h>

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <climits>

// Log system configuration
#ifndef SURFACE_LOG_TAG
#define SURFACE_LOG_TAG "AImGui"
#endif

#ifndef SURFACE_LOG_ENABLE
#define SURFACE_LOG_ENABLE 1  // Set to 0 to completely disable logging
#endif

// Log level control
#ifndef SURFACE_LOG_LEVEL
#define SURFACE_LOG_LEVEL_ERROR   1
#define SURFACE_LOG_LEVEL_WARN    2
#define SURFACE_LOG_LEVEL_INFO    3
#define SURFACE_LOG_LEVEL_DEBUG   4
#define SURFACE_LOG_LEVEL         SURFACE_LOG_LEVEL_DEBUG  // Default DEBUG level
#endif

// Unified log macro definitions
#if SURFACE_LOG_ENABLE
    #define SURFACE_LOG_ERROR(fmt, ...) \
        do { \
            if (SURFACE_LOG_LEVEL >= SURFACE_LOG_LEVEL_ERROR) \
                __android_log_print(ANDROID_LOG_ERROR, SURFACE_LOG_TAG, "[-] " fmt __VA_OPT__(, ) __VA_ARGS__); \
        } while(0)
    
    #define SURFACE_LOG_WARN(fmt, ...) \
        do { \
            if (SURFACE_LOG_LEVEL >= SURFACE_LOG_LEVEL_WARN) \
                __android_log_print(ANDROID_LOG_WARN, SURFACE_LOG_TAG, "[!] " fmt __VA_OPT__(, ) __VA_ARGS__); \
        } while(0)
    
    #define SURFACE_LOG_INFO(fmt, ...) \
        do { \
            if (SURFACE_LOG_LEVEL >= SURFACE_LOG_LEVEL_INFO) \
                __android_log_print(ANDROID_LOG_INFO, SURFACE_LOG_TAG, "[+] " fmt __VA_OPT__(, ) __VA_ARGS__); \
        } while(0)
    
    #define SURFACE_LOG_DEBUG(fmt, ...) \
        do { \
            if (SURFACE_LOG_LEVEL >= SURFACE_LOG_LEVEL_DEBUG) \
                __android_log_print(ANDROID_LOG_DEBUG, SURFACE_LOG_TAG, "[*] " fmt __VA_OPT__(, ) __VA_ARGS__); \
        } while(0)
    
    #define SURFACE_LOG_TRACE(fmt, ...) \
        do { \
            if (SURFACE_LOG_LEVEL >= SURFACE_LOG_LEVEL_DEBUG) \
                __android_log_print(ANDROID_LOG_DEBUG, SURFACE_LOG_TAG, "[=] " fmt __VA_OPT__(, ) __VA_ARGS__); \
        } while(0)
#else
    #define SURFACE_LOG_ERROR(fmt, ...)   ((void)0)
    #define SURFACE_LOG_WARN(fmt, ...)    ((void)0)
    #define SURFACE_LOG_INFO(fmt, ...)    ((void)0)
    #define SURFACE_LOG_DEBUG(fmt, ...)   ((void)0)
    #define SURFACE_LOG_TRACE(fmt, ...)   ((void)0)
#endif

#define ResolveMethod(ClassName, MethodName, Handle, MethodSignature)                                                                    \
    ClassName##__##MethodName = reinterpret_cast<decltype(ClassName##__##MethodName)>(symbolMethod.Find(Handle, MethodSignature));       \
    if (nullptr == ClassName##__##MethodName)                                                                                            \
    {                                                                                                                                    \
        SURFACE_LOG_ERROR("Method not found: %s -> %s::%s", MethodSignature, #ClassName, #MethodName); \
    }

namespace android {
    namespace detail {
        namespace ui {
            // A LayerStack identifies a Z-ordered group of layers. A layer can only be associated to a single
            // LayerStack, but a LayerStack can be associated to multiple displays, mirroring the same content.
            struct LayerStack
            {
                uint32_t id = UINT32_MAX;
            };

            enum class Rotation
            {
                Rotation0 = 0,
                Rotation90 = 1,
                Rotation180 = 2,
                Rotation270 = 3
            };

            // A simple value type representing a two-dimensional size.
            struct Size
            {
                int32_t width = -1;
                int32_t height = -1;
            };

            // Transactional state of physical or virtual display. Note that libgui defines
            // android::DisplayState as a superset of android::ui::DisplayState.
            struct DisplayState
            {
                LayerStack layerStack;
                Rotation orientation = Rotation::Rotation0;
                Size layerStackSpaceRect;
            };

            typedef int64_t nsecs_t; // nano-seconds
            struct DisplayInfo
            {
                uint32_t w{0};
                uint32_t h{0};
                float xdpi{0};
                float ydpi{0};
                float fps{0};
                float density{0};
                uint8_t orientation{0};
                bool secure{false};
                nsecs_t appVsyncOffset{0};
                nsecs_t presentationDeadline{0};
                uint32_t viewportW{0};
                uint32_t viewportH{0};
            };

            enum class DisplayType
            {
                DisplayIdMain = 0,
                DisplayIdHdmi = 1
            };

            struct PhysicalDisplayId
            {
                uint64_t value;
            };

            struct Rect
            {
                int32_t left;
                int32_t top;
                int32_t right;
                int32_t bottom;
            };
        }

        struct String8;

        struct LayerMetadata;

        struct Surface;

        struct SurfaceControl;

        struct SurfaceComposerClientTransaction;

        struct SurfaceComposerClient;

        template <typename any_t>
        struct StrongPointer
        {
            union
            {
                any_t *pointer;
                char padding[sizeof(std::max_align_t)];
            };

            inline any_t *operator->() const { return pointer; }
            inline any_t *get() const { return pointer; }
            inline explicit operator bool() const { return nullptr != pointer; }
        };

        struct Functionals
        {
            struct SymbolMethod
            {
                void *(*Open)(const char *filename, int flag) = nullptr;
                void *(*Find)(void *handle, const char *symbol) = nullptr;
                int (*Close)(void *handle) = nullptr;
            };

            size_t systemVersion = 13;

            void (*RefBase__IncStrong)(void *thiz, void *id) = nullptr;
            void (*RefBase__DecStrong)(void *thiz, void *id) = nullptr;

            void (*String8__Constructor)(void *thiz, const char *const data) = nullptr;
            void (*String8__Destructor)(void *thiz) = nullptr;

            void (*LayerMetadata__Constructor)(void *thiz) = nullptr;
            void (*LayerMetadata__setInt32)(void *thiz, uint32_t key, int32_t value) = nullptr;

            void (*SurfaceComposerClient__Constructor)(void *thiz) = nullptr;
            void (*SurfaceComposerClient__Destructor)(void *thiz) = nullptr;
            StrongPointer<void> (*SurfaceComposerClient__CreateSurface)(void *thiz, void *name, uint32_t w, uint32_t h, int32_t format, uint32_t flags, void *parentHandle, void *layerMetadata, uint32_t *outTransformHint) = nullptr;
            StrongPointer<void> (*SurfaceComposerClient__CreateSurface_and8)(void *thiz, void *name, uint32_t w, uint32_t h, int32_t format, uint32_t flags, void *parentHandle, uint32_t windowType, uint32_t ownerUid) = nullptr;
            StrongPointer<void> (*SurfaceComposerClient__CreateSurface_and9)(void *thiz, void *name, uint32_t w, uint32_t h, int32_t format, uint32_t flags, void *parentHandle, int32_t windowType, int32_t ownerUid) = nullptr;
            StrongPointer<void> (*SurfaceComposerClient__MirrorSurface)(void *thiz, void *mirrorFromSurface) = nullptr;
            StrongPointer<void> (*SurfaceComposerClient__GetInternalDisplayToken)() = nullptr;
            StrongPointer<void> (*SurfaceComposerClient__GetBuiltInDisplay)(ui::DisplayType type) = nullptr;
            int32_t (*SurfaceComposerClient__GetDisplayState)(StrongPointer<void> &display, ui::DisplayState *displayState) = nullptr;
            int32_t (*SurfaceComposerClient__GetDisplayInfo)(StrongPointer<void> &display, ui::DisplayInfo *displayInfo) = nullptr;
            std::vector<ui::PhysicalDisplayId> (*SurfaceComposerClient__GetPhysicalDisplayIds)() = nullptr;
            StrongPointer<void> (*SurfaceComposerClient__GetPhysicalDisplayToken)(ui::PhysicalDisplayId displayId) = nullptr;

            void (*SurfaceComposerClient__OpenGlobalTransaction)() = nullptr;
            void (*SurfaceComposerClient__CloseGlobalTransaction)(bool synchronous) = nullptr;

            void (*SurfaceComposerClient__Transaction__Constructor)(void *thiz) = nullptr;
            void *(*SurfaceComposerClient__Transaction__SetLayer)(void *thiz, StrongPointer<void> &surfaceControl, int32_t z) = nullptr;
            void *(*SurfaceComposerClient__Transaction__SetTrustedOverlay)(void *thiz, StrongPointer<void> &surfaceControl, bool isTrustedOverlay) = nullptr;
            void *(*SurfaceComposerClient__Transaction__SetLayerStack)(void *thiz, StrongPointer<void> &surfaceControl, uint32_t layerStack) = nullptr;
            void *(*SurfaceComposerClient__Transaction__Show)(void *thiz, StrongPointer<void> &surfaceControl) = nullptr;
            void *(*SurfaceComposerClient__Transaction__Hide)(void *thiz, StrongPointer<void> &surfaceControl) = nullptr;
            void *(*SurfaceComposerClient__Transaction__Reparent)(void *thiz, StrongPointer<void> &surfaceControl, StrongPointer<void> &newParentHandle) = nullptr;
            void *(*SurfaceComposerClient__Transaction__SetMatrix)(void *thiz, StrongPointer<void> &surfaceControl, float dsdx, float dtdx, float dtdy, float dsdy) = nullptr;
            void *(*SurfaceComposerClient__Transaction__SetPosition)(void *thiz, StrongPointer<void> &surfaceControl, float x, float y) = nullptr;
            int32_t (*SurfaceComposerClient__Transaction__Apply)(void *thiz, bool synchronous, bool oneWay) = nullptr;

            int32_t (*SurfaceControl__Validate)(void *thiz) = nullptr;
            StrongPointer<Surface> (*SurfaceControl__GetSurface)(void *thiz) = nullptr;
            void (*SurfaceControl__DisConnect)(void *thiz) = nullptr;
            void *(*SurfaceControl__SetLayer)(void *thiz, int32_t z) = nullptr;
            
            // Surface related methods
            void (*Surface__DisConnect)(void *thiz, int32_t api) = nullptr;

            Functionals(const SymbolMethod &symbolMethod)
            {
                std::string systemVersionString(128, 0);

                systemVersionString.resize(__system_property_get("ro.build.version.release", systemVersionString.data()));
                if (!systemVersionString.empty())
                    systemVersion = std::stoi(systemVersionString);

                if (5 > systemVersion)
                {
                    SURFACE_LOG_ERROR("Unsupported system version: %zu", systemVersion);
                    return;
                }

#ifdef __LP64__
                auto libgui = symbolMethod.Open("/system/lib64/libgui.so", RTLD_LAZY);
                auto libutils = symbolMethod.Open("/system/lib64/libutils.so", RTLD_LAZY);
#else
                auto libgui = symbolMethod.Open("/system/lib/libgui.so", RTLD_LAZY);
                auto libutils = symbolMethod.Open("/system/lib/libutils.so", RTLD_LAZY);
#endif
                //libutils
                ResolveMethod(RefBase, IncStrong, libutils, "_ZNK7android7RefBase9incStrongEPKv");
                ResolveMethod(RefBase, DecStrong, libutils, "_ZNK7android7RefBase9decStrongEPKv");

                ResolveMethod(String8, Constructor, libutils, "_ZN7android7String8C2EPKc");
                ResolveMethod(String8, Destructor, libutils, "_ZN7android7String8D2Ev");
                
                //libgui
                if (10 <= systemVersion && 13 >= systemVersion) {
                    ResolveMethod(LayerMetadata, Constructor, libgui, "_ZN7android13LayerMetadataC2Ev");
                    ResolveMethod(LayerMetadata, setInt32, libgui, "_ZN7android13LayerMetadata8setInt32Eji");
                } else if (14 <= systemVersion) {
                    ResolveMethod(LayerMetadata, Constructor, libgui, "_ZN7android3gui13LayerMetadataC2Ev");
                }

                ResolveMethod(SurfaceComposerClient, Constructor, libgui, "_ZN7android21SurfaceComposerClientC2Ev");

                // Select the correct CreateSurface API based on Android version
                if (5 <= systemVersion && 7 >= systemVersion) {
                    // Android 5-7
                    ResolveMethod(SurfaceComposerClient, CreateSurface, libgui, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8Ejjij");
                } else if (8 == systemVersion) {
                    // Android 8
                    ResolveMethod(SurfaceComposerClient, CreateSurface_and8, libgui, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijPNS_14SurfaceControlEjj");
                } else if (9 == systemVersion) {
                    // Android 9
                    ResolveMethod(SurfaceComposerClient, CreateSurface_and9, libgui, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijPNS_14SurfaceControlEii");
                } else if (10 == systemVersion) {
                    // Android 10
                    ResolveMethod(SurfaceComposerClient, CreateSurface, libgui, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijPNS_14SurfaceControlENS_13LayerMetadataE");
                } else if (11 == systemVersion) {
                    // Android 11
                    ResolveMethod(SurfaceComposerClient, CreateSurface, libgui, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijPNS_14SurfaceControlENS_13LayerMetadataEPj");
                } else if (12 <= systemVersion && 13 >= systemVersion) {
                    // Android 12-13
                    ResolveMethod(SurfaceComposerClient, CreateSurface, libgui, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjijRKNS_2spINS_7IBinderEEENS_13LayerMetadataEPj");
                } else if (14 <= systemVersion) {
                    // Android 14+
                    ResolveMethod(SurfaceComposerClient, CreateSurface, libgui, "_ZN7android21SurfaceComposerClient13createSurfaceERKNS_7String8EjjiiRKNS_2spINS_7IBinderEEENS_3gui13LayerMetadataEPj");
                }
                
                // MirrorSurface method - Android 11+
                if (11 <= systemVersion) {
                    ResolveMethod(SurfaceComposerClient, MirrorSurface, libgui, "_ZN7android21SurfaceComposerClient13mirrorSurfaceEPNS_14SurfaceControlE");
                }
                
                // Display related methods - version specific selection
                if (5 <= systemVersion && 9 >= systemVersion) {
                    // Android 5-9 uses GetBuiltInDisplay
                    ResolveMethod(SurfaceComposerClient, GetBuiltInDisplay, libgui, "_ZN7android21SurfaceComposerClient17getBuiltInDisplayEi");
                }
                if (10 <= systemVersion && 13 >= systemVersion) {
                    // Android 10-13 uses GetInternalDisplayToken
                    ResolveMethod(SurfaceComposerClient, GetInternalDisplayToken, libgui, "_ZN7android21SurfaceComposerClient23getInternalDisplayTokenEv");
                }
                if (10 <= systemVersion) {
                    // Android 10+ uses GetPhysicalDisplayIds
                    ResolveMethod(SurfaceComposerClient, GetPhysicalDisplayIds, libgui, "_ZN7android21SurfaceComposerClient21getPhysicalDisplayIdsEv");
                }
                if (12 <= systemVersion) {
                    // Android 12+ uses GetPhysicalDisplayToken
                    ResolveMethod(SurfaceComposerClient, GetPhysicalDisplayToken, libgui, "_ZN7android21SurfaceComposerClient23getPhysicalDisplayTokenENS_17PhysicalDisplayIdE");
                }
                
                // Display state and info retrieval methods
                if (5 <= systemVersion && 11 >= systemVersion) {
                    // Android 5-11 uses GetDisplayInfo
                    ResolveMethod(SurfaceComposerClient, GetDisplayInfo, libgui, "_ZN7android21SurfaceComposerClient14getDisplayInfoERKNS_2spINS_7IBinderEEEPNS_11DisplayInfoE");
                }
                if (11 <= systemVersion) {
                    // Android 11+ uses GetDisplayState
                    ResolveMethod(SurfaceComposerClient, GetDisplayState, libgui, "_ZN7android21SurfaceComposerClient15getDisplayStateERKNS_2spINS_7IBinderEEEPNS_2ui12DisplayStateE");
                }

                // GlobalTransaction methods - Android 5-8 only
                if (5 <= systemVersion && 8 >= systemVersion) {
                    ResolveMethod(SurfaceComposerClient, OpenGlobalTransaction, libgui, "_ZN7android21SurfaceComposerClient21openGlobalTransactionEv");
                    ResolveMethod(SurfaceComposerClient, CloseGlobalTransaction, libgui, "_ZN7android21SurfaceComposerClient22closeGlobalTransactionEb");
                }

                // Transaction related methods - Android 9+
                if (12 <= systemVersion) {
                    ResolveMethod(SurfaceComposerClient__Transaction, Constructor, libgui, "_ZN7android21SurfaceComposerClient11TransactionC2Ev");
                }
                if (9 <= systemVersion) {
                    ResolveMethod(SurfaceComposerClient__Transaction, SetLayer, libgui, "_ZN7android21SurfaceComposerClient11Transaction8setLayerERKNS_2spINS_14SurfaceControlEEEi");
                    ResolveMethod(SurfaceComposerClient__Transaction, Show, libgui, "_ZN7android21SurfaceComposerClient11Transaction4showERKNS_2spINS_14SurfaceControlEEE");
                    ResolveMethod(SurfaceComposerClient__Transaction, Hide, libgui, "_ZN7android21SurfaceComposerClient11Transaction4hideERKNS_2spINS_14SurfaceControlEEE");
                }
                if (12 <= systemVersion) {
                    ResolveMethod(SurfaceComposerClient__Transaction, SetTrustedOverlay, libgui, "_ZN7android21SurfaceComposerClient11Transaction17setTrustedOverlayERKNS_2spINS_14SurfaceControlEEEb");
                    ResolveMethod(SurfaceComposerClient__Transaction, Reparent, libgui, "_ZN7android21SurfaceComposerClient11Transaction8reparentERKNS_2spINS_14SurfaceControlEEES6_");
                }
                if (9 <= systemVersion) {
                    ResolveMethod(SurfaceComposerClient__Transaction, SetMatrix, libgui, "_ZN7android21SurfaceComposerClient11Transaction9setMatrixERKNS_2spINS_14SurfaceControlEEEffff");
                }
                if (5 <= systemVersion) {
                    ResolveMethod(SurfaceComposerClient__Transaction, SetPosition, libgui, "_ZN7android21SurfaceComposerClient11Transaction11setPositionERKNS_2spINS_14SurfaceControlEEEff");
                }
                if (13 <= systemVersion) {
                    ResolveMethod(SurfaceComposerClient__Transaction, SetLayerStack, libgui, "_ZN7android21SurfaceComposerClient11Transaction13setLayerStackERKNS_2spINS_14SurfaceControlEEENS_2ui10LayerStackE");
                }
                
                // Transaction Apply method - version specific selection
                if (9 <= systemVersion && 12 >= systemVersion) {
                    // Android 9-12 uses two-parameter version
                    ResolveMethod(SurfaceComposerClient__Transaction, Apply, libgui, "_ZN7android21SurfaceComposerClient11Transaction5applyEb");
                }
                if (13 <= systemVersion) {
                    // Android 13+ uses three-parameter version
                    ResolveMethod(SurfaceComposerClient__Transaction, Apply, libgui, "_ZN7android21SurfaceComposerClient11Transaction5applyEbb");
                }

                // SurfaceControl related methods
                if (5 <= systemVersion) {
                    ResolveMethod(SurfaceControl, Validate, libgui, "_ZNK7android14SurfaceControl8validateEv");
                }
                
                // SurfaceControl GetSurface method - version specific selection
                if (5 <= systemVersion && 11 >= systemVersion) {
                    // Android 5-11 uses const version
                    ResolveMethod(SurfaceControl, GetSurface, libgui, "_ZNK7android14SurfaceControl10getSurfaceEv");
                }
                if (12 <= systemVersion) {
                    // Android 12+ uses non-const version
                    ResolveMethod(SurfaceControl, GetSurface, libgui, "_ZN7android14SurfaceControl10getSurfaceEv");
                }
                
                // DisConnect method - version specific selection
                if (5 <= systemVersion && 6 >= systemVersion) {
                    // Android 5-6 uses Surface::disconnect
                    ResolveMethod(Surface, DisConnect, libgui, "_ZN7android7Surface10disconnectEi");
                }
                if (7 <= systemVersion) {
                    // Android 7+ uses SurfaceControl::disconnect
                    ResolveMethod(SurfaceControl, DisConnect, libgui, "_ZN7android14SurfaceControl10disconnectEv");
                }
                
                // SetLayer method - version specific selection
                if (5 == systemVersion || 8 == systemVersion) {
                    // Android 5 and 8+ use int version
                    ResolveMethod(SurfaceControl, SetLayer, libgui, "_ZN7android14SurfaceControl8setLayerEi");
                }
                if (6 <= systemVersion && 7 >= systemVersion) {
                    // Android 6-7 use uint version
                    ResolveMethod(SurfaceControl, SetLayer, libgui, "_ZN7android14SurfaceControl8setLayerEj");
                }

                symbolMethod.Close(libutils);
                symbolMethod.Close(libgui);
            }

            static const Functionals &GetInstance(const SymbolMethod &symbolMethod = {.Open = dlopen, .Find = dlsym, .Close = dlclose}) {
                static Functionals functionals(symbolMethod);
                return functionals;
            }
        };

        struct String8
        {
            char data[1024];

            String8(const char *const string)
            {
                Functionals::GetInstance().String8__Constructor(data, string);
            }

            ~String8()
            {
                Functionals::GetInstance().String8__Destructor(data);
            }

            operator void *()
            {
                return reinterpret_cast<void *>(data);
            }
        };

        struct LayerMetadata {
            char data[1024];

            LayerMetadata() {
                if (9 < Functionals::GetInstance().systemVersion) {
                    Functionals::GetInstance().LayerMetadata__Constructor(data);
                }
            }
            
            void setInt32(uint32_t key, int32_t value) {
                Functionals::GetInstance().LayerMetadata__setInt32(data, key, value);            
            }
            
            operator void *() {
                if (9 < Functionals::GetInstance().systemVersion)
                    return reinterpret_cast<void *>(data);
                else
                    return nullptr;
            }
        };

        struct Surface {
        };

        struct SurfaceControl {
            void *data;

            SurfaceControl() : data(nullptr) {}
            SurfaceControl(void *data) : data(data) {}

            int32_t Validate() {
                if (nullptr == data)
                    return 0;

                return Functionals::GetInstance().SurfaceControl__Validate(data);
            }

            Surface *GetSurface() {
                if (nullptr == data)
                    return nullptr;

                auto result = Functionals::GetInstance().SurfaceControl__GetSurface(data);

                return reinterpret_cast<Surface *>(reinterpret_cast<size_t>(result.pointer) + sizeof(std::max_align_t) / 2);
            }

            void DisConnect() {
                if (nullptr == data)
                    return;

                Functionals::GetInstance().SurfaceControl__DisConnect(data);
            }

            void SetLayer(int32_t z) {
                if (nullptr == data)
                    return;

                Functionals::GetInstance().SurfaceControl__SetLayer(data, z);
            }

            void DestroySurface(Surface *surface) {
                if (nullptr == data || nullptr == surface)
                    return;

                Functionals::GetInstance().RefBase__DecStrong(reinterpret_cast<Surface *>(reinterpret_cast<size_t>(surface) - sizeof(std::max_align_t) / 2), this);
                DisConnect();
                Functionals::GetInstance().RefBase__DecStrong(data, this);
            }
        };

        struct SurfaceComposerClientTransaction {
            char data[1024];

            SurfaceComposerClientTransaction() {
                Functionals::GetInstance().SurfaceComposerClient__Transaction__Constructor(data);
            }

            void *SetLayer(StrongPointer<void> &surfaceControl, int32_t z) {
                return Functionals::GetInstance().SurfaceComposerClient__Transaction__SetLayer(data, surfaceControl, z);
            }

            void *SetTrustedOverlay(StrongPointer<void> &surfaceControl, bool isTrustedOverlay) {
                return Functionals::GetInstance().SurfaceComposerClient__Transaction__SetTrustedOverlay(data, surfaceControl, isTrustedOverlay);
            }

            void *SetLayerStack(StrongPointer<void> &surfaceControl, uint32_t layerStack) {
                return Functionals::GetInstance().SurfaceComposerClient__Transaction__SetLayerStack(data, surfaceControl, layerStack);
            }

            void Show(StrongPointer<void> &surfaceControl) {
                Functionals::GetInstance().SurfaceComposerClient__Transaction__Show(data, surfaceControl);
            }

            void Hide(StrongPointer<void> &surfaceControl) {
                Functionals::GetInstance().SurfaceComposerClient__Transaction__Hide(data, surfaceControl);
            }

            void Reparent(StrongPointer<void> &surfaceControl, StrongPointer<void> &newParentHandle) {
                Functionals::GetInstance().SurfaceComposerClient__Transaction__Reparent(data, surfaceControl, newParentHandle);
            }

            void *SetMatrix(StrongPointer<void> &surfaceControl, float dsdx, float dtdx, float dtdy, float dsdy) {
                return Functionals::GetInstance().SurfaceComposerClient__Transaction__SetMatrix(data, surfaceControl, dsdx, dtdx, dtdy, dsdy);
            }

            void SetPosition(StrongPointer<void> &surfaceControl, float x, float y) {
                Functionals::GetInstance().SurfaceComposerClient__Transaction__SetPosition(data, surfaceControl, x, y);
            }

            int32_t Apply(bool synchronous, bool oneWay) {
                if (12 >= Functionals::GetInstance().systemVersion)
                    return reinterpret_cast<int32_t (*)(void *, bool)>(Functionals::GetInstance().SurfaceComposerClient__Transaction__Apply)(data, synchronous);
                else
                    return Functionals::GetInstance().SurfaceComposerClient__Transaction__Apply(data, synchronous, oneWay);
            }
        };

        struct SurfaceComposerClient {
            char data[1024];

            SurfaceComposerClient() {
                Functionals::GetInstance().SurfaceComposerClient__Constructor(data);
                Functionals::GetInstance().RefBase__IncStrong(data, this);
            }

            SurfaceControl CreateSurface(const char *name, int32_t width, int32_t height, uint32_t windowFlags = 0, bool skipScrenshot = false) {
                static void *parentHandle = nullptr;
                parentHandle = nullptr;
                
                String8 windowName(name);
                int32_t pixelFormat = 1; // RGBA_8888
                LayerMetadata layerMetadata{};
                auto systemVersion = Functionals::GetInstance().systemVersion;

                StrongPointer<void> result{};
                
                switch (systemVersion) {
                case 5:
                case 6:
                case 7:
                {
                    result = Functionals::GetInstance().SurfaceComposerClient__CreateSurface(data, windowName, width, height, pixelFormat, windowFlags, parentHandle, layerMetadata, nullptr);
                    break;
                }
                case 8:
                {
                    uint32_t windowType = 0;
                    uint32_t ownerUid = 0;
                    if (skipScrenshot) {
                        windowType = 441731;
                    }
                    result = Functionals::GetInstance().SurfaceComposerClient__CreateSurface_and8(data, windowName, width, height, pixelFormat, windowFlags, parentHandle, windowType, ownerUid);
                    break;
                }
                case 9:
                {
                    int32_t windowType = -1;
                    int32_t ownerUid = -1;
                    if (skipScrenshot) {
                        windowType = 441731;
                    }
                    result = Functionals::GetInstance().SurfaceComposerClient__CreateSurface_and9(data, windowName, width, height, pixelFormat, windowFlags, parentHandle, windowType, ownerUid);
                    break;
                }
                case 10:
                {
                    if (skipScrenshot) {
                        layerMetadata.setInt32(2u, 441731);
                    }
                    result = Functionals::GetInstance().SurfaceComposerClient__CreateSurface(data, windowName, width, height, pixelFormat, windowFlags, parentHandle, layerMetadata, nullptr);
                    break;
                }
                case 11:
                {
                    if (skipScrenshot) {
                        layerMetadata.setInt32(2u, 441731);
                    }
                    result = Functionals::GetInstance().SurfaceComposerClient__CreateSurface(data, windowName, width, height, pixelFormat, windowFlags, parentHandle, layerMetadata, nullptr);
                    break;
                }
                case 12:
                case 13:
                {
                    if (skipScrenshot) {
                        windowFlags |= 0x40; // eSkipScreenshot
                    }
                    result = Functionals::GetInstance().SurfaceComposerClient__CreateSurface(data, windowName, width, height, pixelFormat, windowFlags, &parentHandle, layerMetadata, nullptr);
                    break;
                }
                default: // Android 14+
                {
                    if (skipScrenshot) {
                        windowFlags |= 0x40; // eSkipScreenshot
                    }
                    result = Functionals::GetInstance().SurfaceComposerClient__CreateSurface(data, windowName, width, height, pixelFormat, windowFlags, &parentHandle, layerMetadata, nullptr);
                    break;
                }
                }

                // Check if Surface creation was successful
                if (nullptr == result.get()) {
                    SURFACE_LOG_ERROR("Failed to create surface: %s", name);
                    return {};
                }

                // Apply permission fixes
                if (12 <= systemVersion) {
                    // Android 12+: Use Transaction mechanism to set trusted overlay and highest layer
                    static SurfaceComposerClientTransaction transaction;
                    transaction.SetTrustedOverlay(result, true);
                    transaction.SetLayer(result, INT_MAX);
                    auto applyResult = transaction.Apply(false, true);
                } else if (8 >= systemVersion) {
                    // Android 8 and below: Use global transaction to set layer
                    OpenGlobalTransaction();
                    SurfaceControl{result.get()}.SetLayer(INT_MAX);
                    CloseGlobalTransaction(false);
                }

                return {result.get()};
            }

            bool GetDisplayInfo(ui::DisplayState *displayInfo) {
                static StrongPointer<void> defaultDisplay;

                if (nullptr == defaultDisplay.get()) {
                    if (9 >= Functionals::GetInstance().systemVersion) { // Android 9 and below
                        defaultDisplay = Functionals::GetInstance().SurfaceComposerClient__GetBuiltInDisplay(ui::DisplayType::DisplayIdMain);
                    } else {
                        if (14 > Functionals::GetInstance().systemVersion) { // Android 10-13
                            defaultDisplay = Functionals::GetInstance().SurfaceComposerClient__GetInternalDisplayToken();
                        } else { // Android 14 and above
                            auto displayIds = Functionals::GetInstance().SurfaceComposerClient__GetPhysicalDisplayIds();
                            if (displayIds.empty())
                                return false;

                            defaultDisplay = Functionals::GetInstance().SurfaceComposerClient__GetPhysicalDisplayToken(displayIds[0]);
                        }
                    }
                }

                if (nullptr == defaultDisplay.get())
                    return false;

                if (11 <= Functionals::GetInstance().systemVersion) { // Android 11 and above
                    return 0 == Functionals::GetInstance().SurfaceComposerClient__GetDisplayState(defaultDisplay, displayInfo);
                } else { // Android 10 and below
                    ui::DisplayInfo realDisplayInfo{};
                    if (0 != Functionals::GetInstance().SurfaceComposerClient__GetDisplayInfo(defaultDisplay, &realDisplayInfo))
                        return false;

                    displayInfo->layerStackSpaceRect.width = realDisplayInfo.w;
                    displayInfo->layerStackSpaceRect.height = realDisplayInfo.h;
                    displayInfo->orientation = static_cast<ui::Rotation>(realDisplayInfo.orientation);

                    return true;
                }
            }

            void OpenGlobalTransaction() {
                Functionals::GetInstance().SurfaceComposerClient__OpenGlobalTransaction();
            }

            void CloseGlobalTransaction(bool synchronous) {
                Functionals::GetInstance().SurfaceComposerClient__CloseGlobalTransaction(synchronous);
            }

            SurfaceControl MirrorSurface(SurfaceControl &surface, uint32_t layerStack) {
                using mirror_surfaces_t = std::pair<void *, void *>;
                constexpr auto MirrorSurfacesDeleter = [](mirror_surfaces_t *pair) {
                    SurfaceControl fakeSurface;

                    // Clean up mirror surface
                    if (pair->first) {
                        Functionals::GetInstance().SurfaceControl__DisConnect(pair->first);
                        Functionals::GetInstance().RefBase__DecStrong(pair->first, fakeSurface.data);
                    }

                    // Clean up mirror root surface
                    if (pair->second) {
                        Functionals::GetInstance().SurfaceControl__DisConnect(pair->second);
                        Functionals::GetInstance().RefBase__DecStrong(pair->second, fakeSurface.data);
                    }

                    delete pair;
                };

                using mirror_surfaces_proxy_t = std::unique_ptr<mirror_surfaces_t, decltype(MirrorSurfacesDeleter)>;

                if (13 > Functionals::GetInstance().systemVersion) {
                    return {};
                }

                auto mirrorSurface = Functionals::GetInstance().SurfaceComposerClient__MirrorSurface(data, surface.data);
                if (nullptr == mirrorSurface.get()) {
                    return {};
                }

                // Get display dimensions
                int32_t width = -1, height = -1;
                while (-1 == width || -1 == height) {
                    ui::DisplayState displayInfo{};
                    if (!GetDisplayInfo(&displayInfo))
                        break;

                    width = displayInfo.layerStackSpaceRect.width;
                    height = displayInfo.layerStackSpaceRect.height;
                    break;
                }

                SURFACE_LOG_INFO("Mirror surface size: %d x %d", width, height);

                // Create mirror root surface
                auto mirrorRootName = "MirrorRoot@" + std::to_string(layerStack);
                auto mirrorRootSurface = CreateSurface(mirrorRootName.c_str(), width, height, 0x00004000);
                if (!mirrorRootSurface.data) {
                    return {};
                }

                // Set mirror root surface properties
                static SurfaceComposerClientTransaction transaction;
                static std::vector<mirror_surfaces_proxy_t> mirrorSurfaces;
                
                StrongPointer<void> mirrorRootPtr{mirrorRootSurface.data};
                StrongPointer<void> mirrorPtr{mirrorSurface.get()};
                
                transaction.SetLayer(mirrorRootPtr, INT_MAX);
                transaction.SetLayerStack(mirrorRootPtr, layerStack);
                transaction.Apply(false, true);

                // Set mirror surface properties
                transaction.SetLayerStack(mirrorPtr, layerStack);
                transaction.Show(mirrorPtr);
                transaction.Reparent(mirrorPtr, mirrorRootPtr);
                transaction.Apply(false, true);

                // Add mirror surface pair to management container for proper cleanup
                mirrorSurfaces.emplace_back(
                    new mirror_surfaces_t{mirrorSurface.get(), mirrorRootSurface.data}, 
                    MirrorSurfacesDeleter
                );

                return {mirrorSurface.get()};
            }

            void ZoomSurface(SurfaceControl &surface, float scaleX, float scaleY, uint32_t orientation, bool offset = false) {
                if (nullptr == surface.data)
                    return;

                static SurfaceComposerClientTransaction transaction;
                StrongPointer<void> surfacePtr{surface.data};
                
                // Use SetMatrix to apply scaling transformation
                // SetMatrix parameters: dsdx, dtdx, dtdy, dsdy
                // For scaling: dsdx=scaleX, dtdx=0, dtdy=0, dsdy=scaleY
                if (14 <= Functionals::GetInstance().systemVersion && offset) {
                    float dsdx, dtdx, dtdy, dsdy;
                    switch (orientation) {
                        case 0:
                            dsdx = scaleX;
                            dtdx = 0.0f;
                            dtdy = 0.0f;
                            dsdy = scaleY;
                            break;
                        case 1:
                            dsdx = 0.0f;
                            dtdx = scaleY;
                            dtdy = -scaleX;
                            dsdy = 0.0f;
                            break;
                        case 2:
                            dsdx = -scaleX;
                            dtdx = 0.0f;
                            dtdy = 0.0f;
                            dsdy = -scaleY;
                            break;
                        case 3:
                            dsdx = 0.0f;
                            dtdx = -scaleY;
                            dtdy = scaleX;
                            dsdy = 0.0f;
                            break;
                    }
                    transaction.SetMatrix(surfacePtr, dsdx, dtdx, dtdy, dsdy);
                    SURFACE_LOG_DEBUG("ZoomSurface called with dsdx: %f, dtdx: %f, dtdy: %f, dsdy: %f", dsdx, dtdx, dtdy, dsdy);
                } else {
                    transaction.SetMatrix(surfacePtr, scaleX, 0, 0, scaleY);
                    SURFACE_LOG_DEBUG("ZoomSurface called with scaleX: %f, scaleY: %f", scaleX, scaleY);
                }
                transaction.Apply(false, true);
            }
        };

        struct DumpDisplayInfo
        {
            std::string uniqueId;
            uint32_t currentLayerStack;
            int32_t orientation = 0;
            std::string type;  // 新增 type 字段
            struct
            {
                int32_t left;
                int32_t top;
                int32_t right;
                int32_t bottom;
            } currentLayerStackRect;

            static DumpDisplayInfo MakeFromRawDumpInfo(const std::string_view &uniqueId, const std::string_view &currentLayerStack, const std::string_view &currentLayerStackRect, const std::string_view &orientation = "", const std::string_view &type = "")
            {
                DumpDisplayInfo result;

                result.uniqueId = std::string{uniqueId.begin(), uniqueId.end()};
                result.currentLayerStack = static_cast<uint32_t>(std::stoul(std::string{currentLayerStack.begin(), currentLayerStack.end()}));
                result.orientation = orientation.empty() ? 0 : std::stoi(std::string{orientation.begin(), orientation.end()});
                result.type = std::string{type.begin(), type.end()};  // 设置 type 字段

                auto leftPos = currentLayerStackRect.find("(") + 1;
                auto topPos = currentLayerStackRect.find(", ", leftPos);
                auto rightPos = currentLayerStackRect.find(" - ", topPos + 2);
                auto bottomPos = currentLayerStackRect.find(", ", rightPos + 3);
                auto endPos = currentLayerStackRect.find(")", bottomPos + 2);

                // Don't check it, even though it might cause a crash.
                result.currentLayerStackRect.left = std::stoi(std::string{currentLayerStackRect.begin() + leftPos, currentLayerStackRect.begin() + topPos});
                result.currentLayerStackRect.top = std::stoi(std::string{currentLayerStackRect.begin() + topPos + 2, currentLayerStackRect.begin() + rightPos});
                result.currentLayerStackRect.right = std::stoi(std::string{currentLayerStackRect.begin() + rightPos + 3, currentLayerStackRect.begin() + bottomPos});
                result.currentLayerStackRect.bottom = std::stoi(std::string{currentLayerStackRect.begin() + bottomPos + 2, currentLayerStackRect.begin() + endPos});

                return result;
            }
        };

        inline std::vector<DumpDisplayInfo> ParseDumpDisplayInfo(const std::string_view &dumpDisplayInfo)
        {
            constexpr auto SubStringView = [](const std::string_view &str, std::string_view start, std::string_view end, int startOffset = 0) -> std::string_view
            {
                auto startIt = str.find(start, startOffset);
                if (std::string::npos == startIt)
                    return {};

                auto endIt = str.find(end, startIt + start.size());
                if (std::string::npos == endIt)
                    return {};

                return str.substr(startIt + start.size(), endIt - startIt - start.size());
            };

            std::vector<DumpDisplayInfo> result;

            // DisplayDeviceInfo
            auto dumpDisplayInfoIt = std::string_view::npos;
            while (std::string_view::npos != (dumpDisplayInfoIt = dumpDisplayInfo.find("DisplayDeviceInfo", dumpDisplayInfoIt + 1)))
            {
                // 获取 type 字段
                auto type = SubStringView(dumpDisplayInfo, "type ", ",", dumpDisplayInfoIt);
                auto uniqueId = SubStringView(dumpDisplayInfo, "uniqueId=\"", "\"", dumpDisplayInfoIt);
                auto currentLayerStack = SubStringView(dumpDisplayInfo, "mCurrentLayerStack=", "\n", dumpDisplayInfoIt);
                auto currentLayerStackRect = SubStringView(dumpDisplayInfo, "mCurrentLayerStackRect=", "\n", dumpDisplayInfoIt);
                auto orientation = SubStringView(dumpDisplayInfo, "mCurrentOrientation=", "\n", dumpDisplayInfoIt);

                if ("-1" == currentLayerStack)
                {
                    SURFACE_LOG_ERROR("%s -> Current layer stack is -1, skipping", std::string{uniqueId.begin(), uniqueId.end()}.data());
                    continue;
                }

                if (uniqueId.empty() || currentLayerStack.empty()) {
                    continue;
                }

                result.push_back(DumpDisplayInfo::MakeFromRawDumpInfo(uniqueId, currentLayerStack, currentLayerStackRect, orientation, type));
            }

            return result;
        }

        // Keep the old function for backward compatibility
        inline std::vector<std::pair<std::string, std::string>> ParseDisplayInfo(const std::string_view &displayInfo)
        {
            auto dumpInfos = ParseDumpDisplayInfo(displayInfo);
            std::vector<std::pair<std::string, std::string>> result;
            
            for (const auto& info : dumpInfos) {
                result.emplace_back(info.uniqueId, std::to_string(info.currentLayerStack));
            }
            
            return result;
        }
    }

    class ANativeWindowCreator {
    public:
        struct DisplayInfo {
            int32_t orientation;
            int32_t width;
            int32_t height;
        };

    public:
        static detail::SurfaceComposerClient &GetComposerInstance() {
            static detail::SurfaceComposerClient surfaceComposerClient;
            return surfaceComposerClient;
        }

        static DisplayInfo GetDisplayInfo() {
            auto &surfaceComposerClient = GetComposerInstance();
            detail::ui::DisplayState displayInfo{};

            if (!surfaceComposerClient.GetDisplayInfo(&displayInfo))
                return {};
            
            DisplayInfo local_displayInfo{0};   
            int32_t local_orientation = static_cast<int32_t>(displayInfo.orientation);  
            int32_t local_abs_x = (displayInfo.layerStackSpaceRect.width > displayInfo.layerStackSpaceRect.height ? displayInfo.layerStackSpaceRect.width : displayInfo.layerStackSpaceRect.height);
            int32_t local_abs_y = (displayInfo.layerStackSpaceRect.width < displayInfo.layerStackSpaceRect.height ? displayInfo.layerStackSpaceRect.width : displayInfo.layerStackSpaceRect.height);          
            if (local_orientation == 1 || local_orientation == 3) {
                local_displayInfo.width = local_abs_x;
                local_displayInfo.height = local_abs_y;
            } else {
                local_displayInfo.width = local_abs_y;
                local_displayInfo.height = local_abs_x;
            }
            local_displayInfo.orientation = local_orientation;
            return local_displayInfo;
        }

        static ANativeWindow *Create(const char *name, int32_t width = -1, int32_t height = -1, bool skipScrenshot_ = false) {
            auto &surfaceComposerClient = GetComposerInstance();
            
            // Auto-retrieve display dimensions
            while (-1 == width || -1 == height) {
                detail::ui::DisplayState displayInfo{};
                if (!surfaceComposerClient.GetDisplayInfo(&displayInfo))
                    break;

                width = displayInfo.layerStackSpaceRect.width;
                height = displayInfo.layerStackSpaceRect.height;

                break;
            }

            // Create Surface
            auto surfaceControl = surfaceComposerClient.CreateSurface(name, width, height, 0, skipScrenshot_);
            if (!surfaceControl.data) {
                __android_log_print(ANDROID_LOG_ERROR, "ImGui", "[-] Failed to create surface control for: %s", name);
                return nullptr;
            }

            auto nativeWindow = reinterpret_cast<ANativeWindow *>(surfaceControl.GetSurface());
            if (!nativeWindow) {
                SURFACE_LOG_ERROR("Failed to get native window from surface control");
                return nullptr;
            }

            // Cache Surface controller
            m_cachedSurfaceControl.emplace(nativeWindow, std::move(surfaceControl));
            
            SURFACE_LOG_INFO("ANativeWindow created successfully: %p", nativeWindow);
            return nativeWindow;
        }

        static void Destroy(ANativeWindow *nativeWindow) {
            auto it = m_cachedSurfaceControl.find(nativeWindow);
            if (it == m_cachedSurfaceControl.end())
                return;

            SURFACE_LOG_INFO("Destroying ANativeWindow: %p", nativeWindow);
            
            // Destroy main Surface
            m_cachedSurfaceControl[nativeWindow].DestroySurface(reinterpret_cast<detail::Surface *>(nativeWindow));
            m_cachedSurfaceControl.erase(nativeWindow);
            
            // If this is the last Surface, clear all mirror surfaces
            if (m_cachedSurfaceControl.empty()) {
                SURFACE_LOG_INFO("Last surface destroyed, clearing all mirror surfaces");
                ClearAllMirrorSurfaces();
            }
        }

        // Handle multi-display mirroring, this is the key feature for solving permission issues
        static void ProcessMirrorDisplay() {
            static std::chrono::steady_clock::time_point lastTime{};

            if (13 > detail::Functionals::GetInstance().systemVersion)
                return;

            if (std::chrono::steady_clock::now() - lastTime < std::chrono::seconds(1))
                return;

            // Run "dumpsys display" and get result
            auto pipe = popen("dumpsys display", "r");
            if (!pipe)
            {
                SURFACE_LOG_ERROR("Failed to run dumpsys command");
                return;
            }

            char buffer[512]{};
            std::string dumpDisplayResult;
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
                dumpDisplayResult += buffer;
            pclose(pipe);

            static std::unordered_map<uint32_t, detail::SurfaceControl> cachedLayerStackMirrorSurfaces;
            static std::unordered_map<uint32_t, bool> cachedLayerStackIsOffset;
            static std::unordered_set<uint32_t> cachedLayerStackScales;
            static std::unordered_set<uint32_t> cachedLayerStackPosition;

            auto dumpDisplayInfos = detail::ParseDumpDisplayInfo(dumpDisplayResult);
            for (auto &displayInfo : dumpDisplayInfos)
            {
                // Update multi display layer scale
                static int32_t builtinDisplayWidth = -1, builtinDisplayHeight = -1, builtinDisplayOrientation = 0;
                if (0 == displayInfo.currentLayerStack)
                {
                    builtinDisplayOrientation = displayInfo.orientation;
                    if (displayInfo.orientation == 1 || displayInfo.orientation == 3) {
                        builtinDisplayWidth = displayInfo.currentLayerStackRect.bottom;
                        builtinDisplayHeight = displayInfo.currentLayerStackRect.right;
                    } else {
                        builtinDisplayWidth = displayInfo.currentLayerStackRect.right;
                        builtinDisplayHeight = displayInfo.currentLayerStackRect.bottom;
                    }
                }

                // Process mirror display
                if (0 == displayInfo.currentLayerStack)
                    continue;

                int32_t surfaceDisplayWidth = -1, surfaceDisplayHeight = -1;
                surfaceDisplayWidth = displayInfo.currentLayerStackRect.bottom < displayInfo.currentLayerStackRect.right ? displayInfo.currentLayerStackRect.bottom : displayInfo.currentLayerStackRect.right;
                surfaceDisplayHeight = displayInfo.currentLayerStackRect.bottom > displayInfo.currentLayerStackRect.right ? displayInfo.currentLayerStackRect.bottom : displayInfo.currentLayerStackRect.right;

                bool offset = false;
                if (cachedLayerStackIsOffset.find(displayInfo.currentLayerStack) == cachedLayerStackIsOffset.end())
                {
                    if (builtinDisplayOrientation == 1 || builtinDisplayOrientation == 3)
                    {
                        if (surfaceDisplayHeight == displayInfo.currentLayerStackRect.right)
                        {
                            cachedLayerStackIsOffset.emplace(displayInfo.currentLayerStack, false);
                        } else {
                            cachedLayerStackIsOffset.emplace(displayInfo.currentLayerStack, true);
                        }
                        offset = cachedLayerStackIsOffset[displayInfo.currentLayerStack];
                    }
                } else {
                    offset = cachedLayerStackIsOffset[displayInfo.currentLayerStack];
                }
                

                if (cachedLayerStackMirrorSurfaces.find(displayInfo.currentLayerStack) == cachedLayerStackMirrorSurfaces.end())
                {
                    SURFACE_LOG_INFO("New display layerstack detected: [%s] -> %u", displayInfo.uniqueId.data(), displayInfo.currentLayerStack);

                    for (auto &[_, surfaceControl] : m_cachedSurfaceControl)
                    {
                        auto mirrorLayer = GetComposerInstance().MirrorSurface(surfaceControl, displayInfo.currentLayerStack);
                        if (mirrorLayer.data) {
                            SURFACE_LOG_INFO("Mirror layer created: %p", mirrorLayer.data);
                            cachedLayerStackMirrorSurfaces.emplace(displayInfo.currentLayerStack, std::move(mirrorLayer));
                            break; // Only create one mirror per layerStack
                        }
                    }
                }

                // Handle scaling for different display sizes
                if (-1 != builtinDisplayWidth && -1 != builtinDisplayHeight && cachedLayerStackMirrorSurfaces.find(displayInfo.currentLayerStack) != cachedLayerStackMirrorSurfaces.end())
                {
                    static int32_t lastOrientation = -1;
                    if (cachedLayerStackScales.find(displayInfo.currentLayerStack) == cachedLayerStackScales.end() || 
                        lastOrientation != builtinDisplayOrientation)
                    {
                        auto &mirrorLayer = cachedLayerStackMirrorSurfaces.at(displayInfo.currentLayerStack);

                        float scaleX = static_cast<float>(surfaceDisplayWidth) / builtinDisplayWidth, scaleY = static_cast<float>(surfaceDisplayHeight) / builtinDisplayHeight;
                        if (scaleY < scaleX)
                        {
                            scaleX = scaleY;
                        } else {
                            scaleY = scaleX;
                        }
                        
                        GetComposerInstance().ZoomSurface(mirrorLayer, scaleX, scaleY, builtinDisplayOrientation, offset);
                        cachedLayerStackScales.emplace(displayInfo.currentLayerStack);
                        lastOrientation = builtinDisplayOrientation;
                        SURFACE_LOG_INFO("Update mirror layer scale: %p %f %f", mirrorLayer.data, scaleX, scaleY);
                    }
                }
                // Apply transform to all cached surfaces if needed
                if (cachedLayerStackMirrorSurfaces.find(displayInfo.currentLayerStack) != cachedLayerStackMirrorSurfaces.end()) {
                    auto &mirrorLayer = cachedLayerStackMirrorSurfaces.at(displayInfo.currentLayerStack);
                    if (mirrorLayer.data) {
                        // Apply position transform based on orientation
                        static int32_t lastOrientation = -1;
                        if (builtinDisplayOrientation != lastOrientation || cachedLayerStackPosition.find(displayInfo.currentLayerStack) == cachedLayerStackPosition.end()) {
                            static detail::SurfaceComposerClientTransaction transaction;
                            detail::StrongPointer<void> surfacePtr{mirrorLayer.data};
                            float x = 0, y = 0;
                            float scaleX = static_cast<float>(surfaceDisplayWidth) / builtinDisplayWidth, scaleY = static_cast<float>(surfaceDisplayHeight) / builtinDisplayHeight;
                            int index = 0;
                            if (scaleX <= scaleY) {
                                scaleY = scaleX;
                                index = 1;
                            } else if (scaleY <= scaleX) {
                                scaleX = scaleY;
                                index = 2;
                            }
                            if (14 <= detail::Functionals::GetInstance().systemVersion && offset) {
                                switch (builtinDisplayOrientation) {
                                    case 0: // ROT_0
                                        if (index == 1) {
                                            y = (surfaceDisplayHeight - builtinDisplayHeight * scaleY) / 2;
                                        } else if (index == 2) {
                                            x = (surfaceDisplayWidth - builtinDisplayWidth * scaleX) / 2;
                                        }
                                        break;
                                    case 1: // ROT_90
                                        if (index == 1) {
                                            x = surfaceDisplayWidth - (surfaceDisplayWidth - builtinDisplayWidth * scaleY) / 2;
                                        } else if (index == 2) {
                                            x =  surfaceDisplayWidth;
                                            y = (surfaceDisplayHeight - builtinDisplayHeight * scaleY) / 2;
                                        }
                                        break;
                                    case 2: // ROT_180
                                        if (index == 1) {
                                            x = surfaceDisplayWidth - (surfaceDisplayWidth - builtinDisplayWidth * scaleX) / 2;
                                            y = surfaceDisplayHeight;
                                        } else if (index == 2) {
                                            x = surfaceDisplayWidth;
                                            y = surfaceDisplayHeight - (surfaceDisplayHeight - builtinDisplayHeight * scaleY) / 2;
                                        }
                                        break;
                                    case 3: // ROT_270
                                        if (index == 1) {
                                            x = (surfaceDisplayWidth - builtinDisplayWidth * scaleX) / 2;
                                            y = surfaceDisplayHeight;
                                        } else if (index == 2) {
                                            y = builtinDisplayHeight - (surfaceDisplayHeight - builtinDisplayHeight * scaleX) / 2;
                                        }
                                        break;
                                }
                            } else {
                                if (index == 1) {
                                    if (builtinDisplayOrientation == 1 || builtinDisplayOrientation == 3) {
                                        x = (surfaceDisplayHeight - builtinDisplayHeight * scaleY) / 2;
                                    } else {
                                        y = (surfaceDisplayHeight - builtinDisplayHeight * scaleY) / 2;
                                    }
                                } else if (index == 2) {
                                    if (builtinDisplayOrientation == 1 || builtinDisplayOrientation == 3) {
                                        y = (surfaceDisplayWidth - builtinDisplayWidth * scaleX) / 2;
                                    } else {
                                        x = (surfaceDisplayWidth - builtinDisplayWidth * scaleX) / 2;
                                    }
                                }
                            }
                            transaction.SetPosition(surfacePtr, x, y);
                            transaction.Apply(false, true);
                            lastOrientation = builtinDisplayOrientation;
                            cachedLayerStackPosition.emplace(displayInfo.currentLayerStack);
                            SURFACE_LOG_INFO("Update mirror layer position: %d %f %p %f %f", index, scaleX, mirrorLayer.data, x, y);
                        }
                    }
                }
            }

            lastTime = std::chrono::steady_clock::now();
        }

        // Enable automatic mirror display handling (recommended to call periodically in main loop)
        static void EnableAutoMirrorDisplay(bool enable = true) {
            static bool autoMirrorEnabled = false;
            SURFACE_LOG_INFO("EnableAutoMirrorDisplay called with enable=%s", enable ? "true" : "false");
            autoMirrorEnabled = enable;
            
            if (enable) {
                SURFACE_LOG_INFO("Auto mirror display enabled, calling ProcessMirrorDisplay immediately");
                ProcessMirrorDisplay(); // Execute immediately once
            } else {
                SURFACE_LOG_INFO("Auto mirror display disabled");
            }
        }

        // Get current cached Surface count
        static size_t GetCachedSurfaceCount() {
            return m_cachedSurfaceControl.size();
        }

        // Clear all mirror surfaces
        static void ClearAllMirrorSurfaces() {
            SURFACE_LOG_INFO("Clearing all mirror surfaces...");
            
            // Clear cached mirrors from ProcessMirrorDisplay
            ClearLayerStackMirrorSurfaces();
            
            SURFACE_LOG_INFO("All mirror surfaces cleared");
        }

        // Clear mirror surface for specific LayerStack
        static void ClearMirrorSurfaceForLayerStack(const std::string& layerStack) {
            SURFACE_LOG_INFO("Clearing mirror surface for layerStack: %s", layerStack.c_str());
            
            auto& cachedMirrors = GetLayerStackMirrorSurfaces();
            auto it = cachedMirrors.find(layerStack);
            if (it != cachedMirrors.end()) {
                // SurfaceControl destructor will automatically handle cleanup
                cachedMirrors.erase(it);
                SURFACE_LOG_INFO("Mirror surface for layerStack %s cleared", layerStack.c_str());
            }
        }

        // Get current mirror surface count
        static size_t GetMirrorSurfaceCount() {
            return GetLayerStackMirrorSurfaces().size();
        }

        // Check if mirror exists for specific LayerStack
        static bool HasMirrorForLayerStack(const std::string& layerStack) {
            auto& cachedMirrors = GetLayerStackMirrorSurfaces();
            return cachedMirrors.find(layerStack) != cachedMirrors.end();
        }

        // Complete cleanup when application exits
        static void Cleanup() {
            SURFACE_LOG_INFO("Performing complete cleanup...");
            
            // Clean up all main surfaces
            for (auto& [nativeWindow, surfaceControl] : m_cachedSurfaceControl) {
                SURFACE_LOG_DEBUG("Cleaning up surface: %p", nativeWindow);
                surfaceControl.DestroySurface(reinterpret_cast<detail::Surface *>(nativeWindow));
            }
            m_cachedSurfaceControl.clear();
            
            // Clear all mirror surfaces
            ClearAllMirrorSurfaces();
            
            SURFACE_LOG_INFO("Complete cleanup finished");
        }

    private:
        inline static std::unordered_map<ANativeWindow *, detail::SurfaceControl> m_cachedSurfaceControl;

        // Get reference to LayerStack mirror surface cache
        static std::unordered_map<std::string, detail::SurfaceControl>& GetLayerStackMirrorSurfaces() {
            static std::unordered_map<std::string, detail::SurfaceControl> cachedLayerStackMirrorSurfaces;
            return cachedLayerStackMirrorSurfaces;
        }

        // Clear LayerStack mirror surface cache
        static void ClearLayerStackMirrorSurfaces() {
            auto& cachedMirrors = GetLayerStackMirrorSurfaces();
            size_t count = cachedMirrors.size();
            cachedMirrors.clear();
            SURFACE_LOG_INFO("Cleared %zu layerStack mirror surfaces", count);
        }
    };
}

#undef ResolveMethod

#endif // !A_NATIVE_WINDOW_CREATOR_H