#include "mod_input.h"

#include "menu_checker.h"

namespace vrinput
{
    using namespace vr;

    struct InputCallback
    {
        Hand              device;
        ActionType        type;
        InputCallbackFunc func;

        bool operator==(const InputCallback& a_rhs)
        {
            return (device == a_rhs.device) && (type == a_rhs.type) && (func == a_rhs.func);
        }
    };

    bool block_all_inputs = false;

    float joystick_dpad_threshold = 0.7f;
    float joystick_dpad_threshold_negative = -0.7f;

    std::mutex               callback_lock;
    vr::TrackedDeviceIndex_t g_leftcontroller;
    vr::TrackedDeviceIndex_t g_rightcontroller;

    vr::VRControllerAxis_t joystick[2] = {};
    float                  trigger[2];

    // each button id is mapped to a list of callback funcs
    std::unordered_map<int, std::vector<InputCallback>> callbacks;
    std::unordered_map<vr::EVRButtonId, bool>           buttonStates;

    RE::NiTransform HmdMatrixToNiTransform(const HmdMatrix34_t& hmdMatrix)
    {
        RE::NiTransform niTransform;

        // Copy rotation matrix
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j) { niTransform.rotate.entry[i][j] = hmdMatrix.m[i][j]; }
        }

        // Copy translation vector
        niTransform.translate.x = hmdMatrix.m[0][3];
        niTransform.translate.y = hmdMatrix.m[1][3];
        niTransform.translate.z = hmdMatrix.m[2][3];

        return niTransform;
    }

    HmdMatrix34_t NiTransformToHmdMatrix(const RE::NiTransform& niTransform)
    {
        HmdMatrix34_t hmdMatrix;

        // Copy rotation matrix
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j) { hmdMatrix.m[i][j] = niTransform.rotate.entry[i][j]; }
        }

        // Copy translation vector
        hmdMatrix.m[0][3] = niTransform.translate.x;
        hmdMatrix.m[1][3] = niTransform.translate.y;
        hmdMatrix.m[2][3] = niTransform.translate.z;

        return hmdMatrix;
    }

    void FocusWindow()
    {
        const char* windowName = "Skyrim VR";
        HWND        targetWindow = FindWindowA(NULL, windowName);

        if (targetWindow != NULL)
        {
            RECT windowRect;
            GetWindowRect(targetWindow, &windowRect);

            int centerX = windowRect.left + (windowRect.right - windowRect.left) / 2;
            int centerY = windowRect.top + (windowRect.bottom - windowRect.top) / 2;

            SetCursorPos(centerX, centerY);
            SetForegroundWindow(targetWindow);
        }
    }

    void SendClick(bool a_down, bool isLeft)
    {
        INPUT temp = {};
        temp.type = INPUT_MOUSE;
        if (isLeft) { temp.mi.dwFlags = a_down ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP; }
        else { temp.mi.dwFlags = a_down ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP; }
        temp.mi.dx = 0;
        temp.mi.dy = 0;

        SendInput(1, &temp, sizeof(INPUT));
    }

    void StartBlockingAll() { block_all_inputs = true; }
    void StopBlockingAll() { block_all_inputs = false; }
    bool isBlockingAll() { return block_all_inputs; }

    void AddCallback(const vr::EVRButtonId a_button, const InputCallbackFunc a_callback,
        const Hand hand, const ActionType touch_or_press)
    {
        if (!a_callback || !a_button) return;

        std::scoped_lock lock(callback_lock);
        callbacks[a_button].push_back(InputCallback(hand, touch_or_press, a_callback));
    }

    void RemoveCallback(const vr::EVRButtonId a_button, const InputCallbackFunc a_callback,
        const Hand hand, const ActionType touch_or_press)
    {
        if (!a_callback || !a_button) return;

        std::scoped_lock lock(callback_lock);
        auto             it = std::find(callbacks[a_button].begin(), callbacks[a_button].end(),
                        InputCallback(hand, touch_or_press, a_callback));
        if (it != callbacks[a_button].end()) { callbacks[a_button].erase(it); }
    }

    void ProcessButtonChanges(uint64_t changedMask, uint64_t currentState, bool isLeft, bool touch,
        vr::VRControllerState_t* out)
    {
        // iterate through each of the button codes that we care about
        for (auto buttonID : all_buttons)
        {
            uint64_t bitmask = 1ull << buttonID;

            // check if this button's state has changed and it has any callbacks
            if (bitmask & changedMask && callbacks.contains(buttonID))
            {
                // check whether it was a press or release event
                bool buttonPress = bitmask & currentState;

                const ModInputEvent event_flags = ModInputEvent(static_cast<Hand>(isLeft),
                    static_cast<ActionType>(touch), static_cast<ButtonState>(buttonPress));

                // iterate through callbacks for this button and call if flags match
                for (auto& cb : callbacks[buttonID])
                {
                    if (cb.device == event_flags.device && cb.type == event_flags.touch_or_press)
                    {
                        // the callback tells us if we should block the input
                        if (cb.func(event_flags))
                        {
                            if (buttonPress)  // clear the current state of the button
                            {
                                if (touch) { out->ulButtonTouched &= ~bitmask; }
                                else { out->ulButtonPressed &= ~bitmask; }
                            }
                            else  // set the current state of the button
                            {
                                if (touch) { out->ulButtonTouched |= bitmask; }
                                else { out->ulButtonPressed |= bitmask; }
                            }
                        }
                    }
                }
            }
        }
    }

    /* range: -1.0 to 1.0 for joystick, 0.0 to 1.0 for trigger ( 0 = not touching) */
    inline void ProcessAxisChanges(
        const VRControllerAxis_t& a_joystick, const float& a_trigger, bool isLeft)
    {
        static std::vector<bool> dpad_buffer[2] = { { false, false, false, false },
            { false, false, false, false } };

        std::vector<bool> dpad_temp = {
            (a_joystick.x < joystick_dpad_threshold_negative),
            (a_joystick.y > joystick_dpad_threshold),
            (a_joystick.x > joystick_dpad_threshold),
            (a_joystick.y < joystick_dpad_threshold_negative),
        };

        if (dpad_buffer[isLeft] != dpad_temp)
        {
            for (int id = 0; id < dpad.size(); id++)
            {
                if (dpad_buffer[isLeft][id] != dpad_temp[id])
                {
                    const ModInputEvent event_flags = ModInputEvent(static_cast<Hand>(isLeft),
                        ActionType::kPress, static_cast<ButtonState>((bool)dpad_temp[id]));

                    // iterate through callbacks for this button and call if flags match
                    for (auto& cb : callbacks[id + (int)vr::k_EButton_DPad_Left])
                    {
                        if (cb.device == event_flags.device &&
                            cb.type == event_flags.touch_or_press)
                        {
                            cb.func(event_flags);
                        }
                    }
                }
            }
            dpad_buffer[isLeft] = dpad_temp;
        }

        trigger[isLeft] = a_trigger;
        joystick[isLeft] = a_joystick;
    }

    // handles low level button/trigger events
    bool ControllerInputCallback(vr::TrackedDeviceIndex_t unControllerDeviceIndex,
        const vr::VRControllerState_t* pControllerState, uint32_t unControllerStateSize,
        vr::VRControllerState_t* pOutputControllerState)
    {
        // save last controller input to only do processing on button changes
        static uint64_t prev_pressed[2] = {};
        static uint64_t prev_touched[2] = {};

        // need to remember the last output sent to the game in order to maintain input blocking
        static uint64_t prev_Pressed_out[2] = {};
        static uint64_t prev_touched_out[2] = {};

        if (pControllerState && !menu_checker::IsGameStopped())
        {
            bool isLeft = unControllerDeviceIndex == g_leftcontroller;
            if (isLeft || unControllerDeviceIndex == g_rightcontroller)
            {
                uint64_t pressed_change = prev_pressed[isLeft] ^ pControllerState->ulButtonPressed;
                uint64_t touched_change = prev_touched[isLeft] ^ pControllerState->ulButtonTouched;

                ProcessAxisChanges(
                    pControllerState->rAxis[0], pControllerState->rAxis[1].x, isLeft);

                if (pressed_change)
                {
                    ProcessButtonChanges(pressed_change, pControllerState->ulButtonPressed, isLeft,
                        false, pOutputControllerState);
                    prev_pressed[isLeft] = pControllerState->ulButtonPressed;
                    prev_Pressed_out[isLeft] = pOutputControllerState->ulButtonPressed;
                }
                else { pOutputControllerState->ulButtonPressed = prev_Pressed_out[isLeft]; }

                if (touched_change)
                {
                    ProcessButtonChanges(touched_change, pControllerState->ulButtonTouched, isLeft,
                        true, pOutputControllerState);
                    prev_touched[isLeft] = pControllerState->ulButtonTouched;
                    prev_touched_out[isLeft] = pOutputControllerState->ulButtonTouched;
                }
                else { pOutputControllerState->ulButtonTouched = prev_touched_out[isLeft]; }

                if (isBlockingAll())
                {
                    pOutputControllerState->ulButtonPressed = 0;
                    pOutputControllerState->ulButtonTouched = 0;
                    pOutputControllerState->rAxis->x = 0.0f;
                    pOutputControllerState->rAxis->y = 0.0f;
                }
            }
        }
        return true;
    }

    // handles device poses
    vr::EVRCompositorError ControllerPoseCallback(VR_ARRAY_COUNT(unRenderPoseArrayCount)
                                                      vr::TrackedDevicePose_t* pRenderPoseArray,
        uint32_t                                                      unRenderPoseArrayCount,
        VR_ARRAY_COUNT(unGamePoseArrayCount) vr::TrackedDevicePose_t* pGamePoseArray,
        uint32_t                                                      unGamePoseArrayCount)
    {
        static vr::TrackedDevicePose_t simple_buffer[2] = {};

        for (auto isLeft : { true, false })
        {
            auto device = isLeft ? g_leftcontroller : g_rightcontroller;
        }
        return vr::EVRCompositorError::VRCompositorError_None;
    }
}
