#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>
#include <linux/input.h>
#include <linux/uinput.h>
#include <vector>
#include <thread>
#include <unordered_map>
#include <array>
#include "spinlock.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "TouchHelperA.h"
#include "Utils.h"

#define maxE 5
#define maxF 10
#define UNGRAB 0
#define GRAB 1

namespace Touch {

    // Per-touch-sequence target. A finger is bound to a single target from
    // press to release; this avoids "drag from UI into game (or vice versa)"
    // mid-gesture jumps. None = not yet decided.
    //
    // Decision is made from the current touch position against visible ImGui
    // windows, so taps outside the menu are forwarded to the system immediately.
    enum class TouchOwner : uint8_t { None, ImGui, System };
    static std::array<std::array<TouchOwner, maxF>, maxE> finger_owner{};

    static std::array<std::array<bool, maxF>, maxE> finger_forwarded_to_system{};

    static struct {
        input_event downEvent[2]{{{}, EV_KEY, BTN_TOUCH,       1}, {{}, EV_KEY, BTN_TOOL_FINGER, 1}};
        input_event event[512]{0};
    } input;

    static My_Vector2 touch_scale;

    static My_Vector2 screenSize;

    static std::vector<Device> devices;

    static int nowfd;

    static int orientation = 0;

    static bool initialized = false;

    static bool readOnly = false;

    static bool otherTouch = false;

    static std::function<void(std::vector<Device> *)> callback;

    static spinlock lock;

    static bool IsPointInsideImGuiWindow(const ImVec2& pos) {
        ImGuiContext* ctx = ImGui::GetCurrentContext();
        if (!ctx) {
            return false;
        }
        for (int n = ctx->Windows.Size - 1; n >= 0; --n) {
            ImGuiWindow* window = ctx->Windows[n];
            if (!window || !window->WasActive || window->Hidden) {
                continue;
            }
            if ((window->Flags & ImGuiWindowFlags_NoInputs) != 0) {
                continue;
            }
            const ImRect rect(window->OuterRectClipped.Min, window->OuterRectClipped.Max);
            if (rect.Contains(pos)) {
                return true;
            }
        }
        return false;
    }

    static bool ShouldUploadFinger(size_t deviceIndex, size_t fingerIndex, bool systemOnly) {
        if (deviceIndex >= devices.size() || fingerIndex >= maxF)
            return false;

        const touchObj &finger = devices[deviceIndex].Finger[fingerIndex];
        if (!finger.isDown)
            return false;

        if (!systemOnly)
            return true;

        return deviceIndex < maxE &&
               fingerIndex < maxF &&
               finger_owner[deviceIndex][fingerIndex] == TouchOwner::System;
    }

    static void UploadImpl(bool systemOnly) {
        static bool isFirstDown = true;
        int tmpCnt = 0, tmpCnt2 = 0;
        for (size_t deviceIndex = 0; deviceIndex < devices.size(); ++deviceIndex) {
            for (size_t fingerIndex = 0; fingerIndex < maxF; ++fingerIndex) {
                if (!ShouldUploadFinger(deviceIndex, fingerIndex, systemOnly))
                    continue;

                const touchObj &finger = devices[deviceIndex].Finger[fingerIndex];
                if (tmpCnt2++ > 20) {
                    goto finish;
                }
                input.event[tmpCnt].type = EV_ABS;
                input.event[tmpCnt].code = ABS_X;
                input.event[tmpCnt].value = (int) finger.pos.x;
                tmpCnt++;

                input.event[tmpCnt].type = EV_ABS;
                input.event[tmpCnt].code = ABS_Y;
                input.event[tmpCnt].value = (int) finger.pos.y;
                tmpCnt++;

                input.event[tmpCnt].type = EV_ABS;
                input.event[tmpCnt].code = ABS_MT_POSITION_X;
                input.event[tmpCnt].value = (int) finger.pos.x;
                tmpCnt++;

                input.event[tmpCnt].type = EV_ABS;
                input.event[tmpCnt].code = ABS_MT_POSITION_Y;
                input.event[tmpCnt].value = (int) finger.pos.y;
                tmpCnt++;

                input.event[tmpCnt].type = EV_ABS;
                input.event[tmpCnt].code = ABS_MT_TRACKING_ID;
                input.event[tmpCnt].value = finger.id;
                tmpCnt++;

                input.event[tmpCnt].type = EV_SYN;
                input.event[tmpCnt].code = SYN_MT_REPORT;
                input.event[tmpCnt].value = 0;
                tmpCnt++;
            }
        }
        finish:
        bool is = false;
        if (tmpCnt == 0) {
            input.event[tmpCnt].type = EV_SYN;
            input.event[tmpCnt].code = SYN_MT_REPORT;
            input.event[tmpCnt].value = 0;
            tmpCnt++;
            if (!isFirstDown) {
                isFirstDown = true;
                input.event[tmpCnt].type = EV_KEY;
                input.event[tmpCnt].code = BTN_TOUCH;
                input.event[tmpCnt].value = 0;
                tmpCnt++;
                input.event[tmpCnt].type = EV_KEY;
                input.event[tmpCnt].code = BTN_TOOL_FINGER;
                input.event[tmpCnt].value = 0;
                tmpCnt++;
            }
        } else {
            is = true;
        }
        input.event[tmpCnt].type = EV_SYN;
        input.event[tmpCnt].code = SYN_REPORT;
        input.event[tmpCnt].value = 0;
        tmpCnt++;

        if (is && isFirstDown) {
            isFirstDown = false;
            write(nowfd, &input, sizeof(struct input_event) * (tmpCnt + 2));
        } else {
            write(nowfd, input.event, sizeof(struct input_event) * tmpCnt);
        }
    }

    void Upload() {
        UploadImpl(false);
    }

    static void UploadSystemTouches() {
        UploadImpl(true);
    }


    /*void *TypeB(void *arg) {
        int i = (int) (long) arg;
        Device &device = devices[i];
        int latest = 0;
        input_event inputEvent[64]{0};

        while (Touch_initialized) {
            auto readSize = (int32_t) read(origfd[i], inputEvent, sizeof(inputEvent));
            if (readSize <= 0 || (readSize % sizeof(input_event)) != 0) {
                continue;
            }
            size_t count = size_t(readSize) / sizeof(input_event);
            for (size_t j = 0; j < count; j++) {
                input_event &ie = inputEvent[j];
                if (latest < 0)
                    latest = 0;
                if (latest >= 10)
                    continue;
                if (ie.code == ABS_MT_TRACKING_ID) {
                    if (ie.value < 0) {
                        Finger[i][latest].isDown = false;
                    } else {
                        Finger[i][latest].isDown = true;
                    }
                    Finger[i][latest].id = (i * 2 + 1) * maxF + ie.value;
                    continue;
                }
                if (ie.code == ABS_MT_POSITION_X) {
                    Finger[i][latest].isDown = true;
                    Finger[i][latest].x = (int) (ie.value * S2TX);
                    continue;
                }
                if (ie.code == ABS_MT_POSITION_Y) {
                    Finger[i][latest].isDown = true;
                    Finger[i][latest].y = (int) (ie.value * S2TY);
                    continue;
                }
                if (ie.code == SYN_MT_REPORT) {
                    latest += 1;
                    continue;
                }
                if (ie.code == SYN_REPORT) {
                    Upload();
                    memset(&Finger[i][0], 0, sizeof(Finger) * 10);
                    latest = -1;
                    continue;
                }
            }
        }
        return nullptr;
    }*/

    static void *TypeA(void *arg) {
        int i = (int) (long) arg;
        Device &device = devices[i];

        int latest = 0;
        input_event inputEvent[64]{0};

        while (initialized) {
            auto readSize = (int32_t) read(device.fd, inputEvent, sizeof(inputEvent));
            if (readSize <= 0 || (readSize % sizeof(input_event)) != 0) {
                continue;
            }
            size_t count = size_t(readSize) / sizeof(input_event);

            lock.lock();
            for (size_t j = 0; j < count; j++) {
                input_event &ie = inputEvent[j];
                if (ie.type == EV_ABS) {
                    if (ie.code == ABS_MT_SLOT) {
                        latest = ie.value;
                        continue;
                    }
                    if (ie.code == ABS_MT_TRACKING_ID) {
                        if (ie.value == -1) {
                            device.Finger[latest].isDown = false;
                            // Ownership is cleared below in SYN_REPORT
                            // after the release frame has been dispatched,
                            // so the system still gets a proper up-event.
                        } else {
                            device.Finger[latest].id = (i * 2 + 1) * maxF + latest;
                            device.Finger[latest].isDown = true;
                            // New press — leave owner as None until SYN_REPORT
                            // decides from the current touch position.
                            if ((size_t) i < maxE && (size_t) latest < maxF) {
                                finger_owner[i][latest] = TouchOwner::None;
                                finger_forwarded_to_system[i][latest] = false;
                            }
                        }
                        continue;
                    }
                    if (ie.code == ABS_MT_POSITION_X) {
                        device.Finger[latest].id = (i * 2 + 1) * maxF + latest;
                        device.Finger[latest].pos.x = (float) ie.value * device.S2TX;
                        continue;
                    }
                    if (ie.code == ABS_MT_POSITION_Y) {
                        device.Finger[latest].id = (i * 2 + 1) * maxF + latest;
                        device.Finger[latest].pos.y = (float) ie.value * device.S2TY;
                        continue;
                    }
                }
                if (ie.code == SYN_REPORT) {
                    TouchOwner *owner = nullptr;
                    if ((size_t) i < maxE && (size_t) latest < maxF) {
                        owner = &finger_owner[i][latest];
                    }

                    const bool nowDown = device.Finger[latest].isDown;
                    const bool imguiReady =
                        (ImGui::GetCurrentContext() != nullptr);
                    bool pointInsideImGui = false;

                    if (imguiReady) {
                        ImGuiIO &io = ImGui::GetIO();

                        // Always keep io.MousePos in sync. Ownership is
                        // decided from the current touch position instead of
                        // waiting for stale WantCaptureMouse to refresh.
                        if (nowDown) {
                            auto pos = Touch2Screen(device.Finger[latest].pos);
                            const ImVec2 mousePos(pos.x, pos.y);
                            io.MousePos = mousePos;
                            pointInsideImGui = IsPointInsideImGuiWindow(mousePos);
                        }
                    }

                    // Sequence ownership decision.
                    // Use the current touch position against visible ImGui
                    // windows so touches outside the menu go to the system
                    // immediately.
                    if (owner && *owner == TouchOwner::None && nowDown) {
                        *owner = pointInsideImGui ? TouchOwner::ImGui : TouchOwner::System;
                    }

                    // io.MouseDown must mirror the ownership and current
                    // press state. Forcing it to false whenever the finger
                    // is owned by the system (or undecided / released)
                    // prevents ImGui from getting "stuck" with MouseDown=
                    // true after a press that ended up going to the system
                    // — which was the second half of the visible bug
                    // (UI becoming unclickable after one system tap).
                    if (imguiReady) {
                        ImGui::GetIO().MouseDown[0] =
                            (owner && *owner == TouchOwner::ImGui && nowDown);
                    }

                    // Forward to the uinput virtual device only when the
                    // sequence is definitively owned by the system. Sequences
                    // still pending an ownership decision are NOT forwarded
                    // — they would otherwise leak UI taps through to the
                    // underlying app for a frame or two.
                    if (!readOnly && owner && *owner == TouchOwner::System) {
                        if (callback) {
                            callback(&devices);
                        } else {
                            UploadSystemTouches();
                        }
                        if ((size_t) i < maxE && (size_t) latest < maxF) {
                            finger_forwarded_to_system[i][latest] = true;
                        }
                    }

                    // Safety fallback: if a release arrives while ownership
                    // is still undecided and the point is outside ImGui, replay
                    // a synthetic system tap so non-UI clicks are not swallowed.
                    if (!readOnly && owner && *owner == TouchOwner::None && !nowDown &&
                        (size_t) i < maxE && (size_t) latest < maxF &&
                        !finger_forwarded_to_system[i][latest] &&
                        !pointInsideImGui) {
                        *owner = TouchOwner::System;
                        device.Finger[latest].isDown = true;
                        if (callback) {
                            callback(&devices);
                        } else {
                            UploadSystemTouches();
                        }
                        device.Finger[latest].isDown = false;
                        if (callback) {
                            callback(&devices);
                        } else {
                            UploadSystemTouches();
                        }
                        finger_forwarded_to_system[i][latest] = true;
                    }

                    // Release frame has been dispatched; reset ownership
                    // so the next press is decided fresh.
                    if (owner && !nowDown) {
                        *owner = TouchOwner::None;
                        if ((size_t) i < maxE && (size_t) latest < maxF) {
                            finger_forwarded_to_system[i][latest] = false;
                        }
                    }
                    continue;
                }
            }
            lock.unlock();

        }
        return nullptr;
    }

    static bool checkDeviceIsTouch(int fd) {
        uint8_t *bits = NULL;
        ssize_t bits_size = 0;
        int res, j, k;
        bool itmp = false, itmp2 = false, itmp3 = false;
        struct input_absinfo abs{};
        while (true) {
            res = ioctl(fd, EVIOCGBIT(EV_ABS, bits_size), bits);
            if (res < bits_size)
                break;
            bits_size = res + 16;
            bits = (uint8_t *) realloc(bits, bits_size * 2);
        }
        for (j = 0; j < res; j++) {
            for (k = 0; k < 8; k++)
                if (bits[j] & 1 << k && ioctl(fd, EVIOCGABS(j * 8 + k), &abs) == 0) {
                    if (j * 8 + k == ABS_MT_SLOT) {
                        itmp = true;
                        continue;
                    }
                    if (j * 8 + k == ABS_MT_POSITION_X) {
                        itmp2 = true;
                        continue;
                    }
                    if (j * 8 + k == ABS_MT_POSITION_Y) {
                        itmp3 = true;
                        continue;
                    }
                }
        }
        free(bits);
        return itmp && itmp2 && itmp3;
    }

    bool Init(const My_Vector2 &s, bool p_readOnly) {
        Close();
        devices.clear();
        My_Vector2 size = s;
        readOnly = p_readOnly;
        if (size.x > size.y) {
            screenSize = size;
        } else {
            screenSize = {size.y, size.x};
        }
        DIR *dir = opendir("/dev/input/");
        if (!dir) {
            return false;
        }

        dirent *ptr = NULL;
        int eventCount = 0;
        while ((ptr = readdir(dir)) != NULL) {
            if (strstr(ptr->d_name, "event"))
                eventCount++;
        }

        char temp[128];
        for (int i = 0; i <= eventCount; i++) {
            sprintf(temp, "/dev/input/event%d", i);
            int fd = open(temp, O_RDWR);
            if (fd < 0) {
                continue;
            }
            if (checkDeviceIsTouch(fd)) {
                Device device{};
                if (ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &device.absX) == 0
                    && ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &device.absY) == 0) {
                    device.fd = fd;
                    if (!readOnly) {
                        ioctl(fd, EVIOCGRAB, GRAB);
                    }
                    devices.push_back(device);
                }
            } else {
                close(fd);
            }
        }

        if (devices.empty()) {
            puts("获取屏幕驱动失败");
            return false;
        }
        //LOGD("device count: %zu", devices.size());

        int screenX = devices[0].absX.maximum;
        int screenY = devices[0].absY.maximum;

        if (!readOnly) {
            struct uinput_user_dev ui_dev;
            nowfd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
            if (nowfd <= 0) {
                return false;
            }

            int string_len = rand() % 10 + 5;
            char string[string_len];
            memset(&ui_dev, 0, sizeof(ui_dev));

            genRandomString(string, string_len);
            strncpy(ui_dev.name, string, UINPUT_MAX_NAME_SIZE);

            ui_dev.id.bustype = 0;
            ui_dev.id.vendor = rand() % 10 + 5;
            ui_dev.id.product = rand() % 10 + 5;
            ui_dev.id.version = rand() % 10 + 5;

            ioctl(nowfd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);

            ioctl(nowfd, UI_SET_EVBIT, EV_ABS);
            ioctl(nowfd, UI_SET_ABSBIT, ABS_X);
            ioctl(nowfd, UI_SET_ABSBIT, ABS_Y);
            ioctl(nowfd, UI_SET_ABSBIT, ABS_MT_POSITION_X);
            ioctl(nowfd, UI_SET_ABSBIT, ABS_MT_POSITION_Y);
            ioctl(nowfd, UI_SET_ABSBIT, ABS_MT_TRACKING_ID);
            ioctl(nowfd, UI_SET_EVBIT, EV_SYN);
            ioctl(nowfd, UI_SET_EVBIT, EV_KEY);
            ioctl(nowfd, UI_SET_KEYBIT, BTN_TOOL_FINGER);
            ioctl(nowfd, UI_SET_KEYBIT, BTN_TOUCH);

            genRandomString(string, string_len);
            ioctl(nowfd, UI_SET_PHYS, string);

            int fd = devices[0].fd;
            {
                struct input_id id{};
                if (ioctl(fd, EVIOCGID, &id) == 0) {
                    ui_dev.id.bustype = id.bustype;
                    ui_dev.id.vendor = id.vendor;
                    ui_dev.id.product = id.product;
                    ui_dev.id.version = id.version;
                }
                uint8_t *bits = NULL;
                ssize_t bits_size = 0;
                int res, j, k;
                while (1) {
                    res = ioctl(fd, EVIOCGBIT(EV_KEY, bits_size), bits);
                    if (res < bits_size)
                        break;
                    bits_size = res + 16;
                    bits = (uint8_t *) realloc(bits, bits_size * 2);
                }
                for (j = 0; j < res; j++) {
                    for (k = 0; k < 8; k++)
                        if (bits[j] & 1 << k) {
                            if (j * 8 + k == BTN_TOUCH || j * 8 + k == BTN_TOOL_FINGER)
                                continue;
                            ioctl(nowfd, UI_SET_KEYBIT, j * 8 + k);
                        }
                }
                free(bits);
            }
            ui_dev.absmin[ABS_MT_POSITION_X] = 0;
            ui_dev.absmax[ABS_MT_POSITION_X] = screenX;
            ui_dev.absmin[ABS_MT_POSITION_Y] = 0;
            ui_dev.absmax[ABS_MT_POSITION_Y] = screenY;
            ui_dev.absmin[ABS_X] = 0;
            ui_dev.absmax[ABS_X] = screenX;
            ui_dev.absmin[ABS_Y] = 0;
            ui_dev.absmax[ABS_Y] = screenY;
            ui_dev.absmin[ABS_MT_TRACKING_ID] = 0;
            ui_dev.absmax[ABS_MT_TRACKING_ID] = 65535;
            write(nowfd, &ui_dev, sizeof(ui_dev));

            if (ioctl(nowfd, UI_DEV_CREATE)) {
                return false;
            }
        }
        initialized = true;

        pthread_t t;
        for (int i = 0; i < devices.size(); i++) {
            devices[i].S2TX = (float) screenX / (float) devices[i].absX.maximum;
            devices[i].S2TY = (float) screenY / (float) devices[i].absY.maximum;
            pthread_create(&t, nullptr, TypeA, (void *) (long) i);
        }
        if (size.x > size.y) {
            std::swap(size.x, size.y);
        }
        if (otherTouch) {
            std::swap(size.x, size.y);
        }
        touch_scale.x = (float) screenX / size.x;
        touch_scale.y = (float) screenY / size.y;

        //system("chmod 000 -R /proc/bus/input/*");
        return true;
    }

    void Close() {
        if (initialized) {
            for (auto &device: devices) {
                if (!readOnly)
                    ioctl(device.fd, EVIOCGRAB, UNGRAB);
                close(device.fd);
                device.fd = 0;
            }
            if (nowfd > 0) {
                ioctl(nowfd, UI_DEV_DESTROY);
                close(nowfd);
                nowfd = 0;
            }
            memset(input.event, 0, sizeof(input.event));
            initialized = false;
            devices.clear();
        }
    }

    void Down(float x, float y) {
        lock.lock();
        touchObj &touch = devices[0].Finger[9];
        touch.id = 19;
        touch.pos = My_Vector2(x, y) * touch_scale;
        touch.isDown = true;
        Upload();
        lock.unlock();
    }

    void Move(touchObj *touch, float x, float y) {
        lock.lock();
        touch->pos = My_Vector2(x, y) * touch_scale;
        Upload();
        lock.unlock();
    }

    void Move(float x, float y) {
        Down(x, y);
    }

    void Up() {
        lock.lock();
        touchObj &touch = devices[0].Finger[9];
        touch.isDown = false;
        Upload();
        lock.unlock();
    }

    void SetCallBack(const std::function<void(std::vector<Device> *)> &cb) {
        callback = cb;
    }

    My_Vector2 Touch2Screen(const My_Vector2 &coord) {
        float x = coord.x, y = coord.y;
        float xt = x / touch_scale.x;
        float yt = y / touch_scale.y;

        if (otherTouch) {
            switch (orientation) {
                case 1:
                    x = xt;
                    y = yt;
                    break;
                case 2:
                    y = yt;
                    x = screenSize.y - xt;
                    break;
                case 3:
                    x = screenSize.y - xt;
                    y = screenSize.x - yt;
                    break;
                default:
                    y = xt;
                    x = screenSize.y - yt;
                    break;
            }
        } else {
            switch (orientation) {
                case 1:
                    x = yt;
                    y = screenSize.y - xt;
                    break;
                case 2:
                    x = screenSize.y - xt;
                    y = screenSize.x - yt;
                    break;
                case 3:
                    y = xt;
                    x = screenSize.x - yt;
                    break;
                default:
                    x = xt;
                    y = yt;
                    break;
            }
        }
        return {x, y};
    }

    My_Vector2 GetScale() {
        return touch_scale;
    }

    void setOrientation(int o) {
        orientation = o;
    }

    void setOtherTouch(bool p_otherTouch) {
        otherTouch = p_otherTouch;
    }
}
