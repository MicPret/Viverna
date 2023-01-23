#include <viverna/core/Input.hpp>
#include <viverna/core/Debug.hpp>

#include <android/input.h>
#include <android_native_app_glue.h>

#include <cstdint>

namespace verna {

static uint32_t touch;

static void SetTouch(bool click) {
    constexpr uint32_t mask = ~(1 << 31);
    touch = (touch & mask) | (static_cast<uint32_t>(click) << 31);
}
static void SetTouchPos(uint32_t x, uint32_t y) {
    uint32_t t = touch & (1 << 31);
    touch = t | (x << 15) | y;
}
static bool GetTouch() {
    return (touch >> 31) != 0;
}
static void GetTouchPos(unsigned& x, unsigned& y) {
    constexpr uint32_t mask = ~(1 << 31);
    uint32_t t = touch & mask;
    x = t >> 15;
    y = t & 0x7FFF;
}

static int32_t HandleInput(struct android_app* app, AInputEvent* event) {
    float x, y;
    int32_t event_type = AInputEvent_getType(event);
    int32_t action;
    switch (event_type) {
        case AINPUT_EVENT_TYPE_MOTION:
            action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
            switch (action) {
                case AMOTION_EVENT_ACTION_DOWN:
                    SetTouch(true);
                    break;
                case AMOTION_EVENT_ACTION_UP:
                    SetTouch(false);
                    break;
                default:
                    break;
            }
            x = AMotionEvent_getX(event, 0);
            y = AMotionEvent_getY(event, 0);
            SetTouchPos(static_cast<uint32_t>(x), static_cast<uint32_t>(y));
            VERNA_LOGI(std::to_string(touch));
            return 1;
        default:
            return 0;
    }
}

void InitializeInput(VivernaState& state) {
    if (state.GetFlag(VivernaState::INPUT_INITIALIZED_FLAG))
        return;
    if (state.native_app == nullptr) {
        VERNA_LOGE("InitializeInput must be called after InitializeWindow!");
        state.SetFlag(VivernaState::ERROR_FLAG, true);
        return;
    }
    struct android_app* app =
        static_cast<struct android_app*>(state.native_app);
    app->onInputEvent = HandleInput;
    state.SetFlag(VivernaState::INPUT_INITIALIZED_FLAG, true);
    VERNA_LOGI("Input initialized!");
}

void TerminateInput(VivernaState& state) {
    if (!state.GetFlag(VivernaState::INPUT_INITIALIZED_FLAG))
        return;
    if (state.native_app == nullptr) {
        VERNA_LOGE("TerminateInput must be called before TerminateWindow!");
        state.SetFlag(VivernaState::ERROR_FLAG, true);
        return;
    }
    struct android_app* app =
        static_cast<struct android_app*>(state.native_app);
    app->onInputEvent = nullptr;
    state.SetFlag(VivernaState::INPUT_INITIALIZED_FLAG, false);
    VERNA_LOGI("Input terminated!");
}

bool ClickListener::Pressed(unsigned& pos_x, unsigned& pos_y) const {
    GetTouchPos(pos_x, pos_y);
    return GetTouch();
}

}  // namespace verna