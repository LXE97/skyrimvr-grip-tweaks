#pragma once
#include "VR/PapyrusVRAPI.h"

namespace vrinput
{
	extern vr::TrackedDeviceIndex_t g_leftcontroller;
	extern vr::TrackedDeviceIndex_t g_rightcontroller;

	constexpr const char* kLeftHandNodeName = "NPC L Hand [LHnd]";
	constexpr const char* kRightHandNodeName = "NPC R Hand [RHnd]";
	constexpr const char* kControllerNodeName[2] = { kRightHandNodeName, kLeftHandNodeName };

	constexpr std::array all_buttons{
		// vr::k_EButton_System, don't think this will ever be useful
		vr::k_EButton_ApplicationMenu,
		vr::k_EButton_Grip,
		vr::k_EButton_A,
		vr::k_EButton_Knuckles_JoyStick,
		vr::k_EButton_SteamVR_Touchpad,
		vr::k_EButton_SteamVR_Trigger,
	};

	/* opencomposite automatically generates dpad events for joystick movement, but SteamVR doesn't,
	* so these are treated separately and generated from joystick axes internally */
	constexpr std::array dpad{
		vr::k_EButton_DPad_Left,
		vr::k_EButton_DPad_Up,
		vr::k_EButton_DPad_Right,
		vr::k_EButton_DPad_Down,
	};

	enum class Hand
	{
		kRight = 0,
		kLeft,
		kBoth
	};
	enum class ActionType
	{
		kPress = 0,  // dpad(joystick) events are always kPress
		kTouch
	};
	enum class ButtonState
	{
		kButtonUp = 0,
		kButtonDown
	};

	struct ModInputEvent
	{
		Hand        device;
		ActionType  touch_or_press;
		ButtonState button_state;

		bool operator==(const ModInputEvent& a_rhs)
		{
			return (device == a_rhs.device) && (touch_or_press == a_rhs.touch_or_press) &&
				(button_state == a_rhs.button_state);
		}
	};

	// returns: true if input that triggered the event should be blocked
	// If the press is blocked, then we don't need to block the release
	typedef bool (*InputCallbackFunc)(const ModInputEvent& e);

	void FocusWindow();
	void SendClick(bool a_down, bool isLeft);

	void StartBlockingAll();
	void StopBlockingAll();
	bool isBlockingAll();

	void StartSmoothing();
	void StopSmoothing();

	void AddCallback(const vr::EVRButtonId a_button, const InputCallbackFunc a_callback,
		const Hand hand, const ActionType touch_or_press);
	void RemoveCallback(const vr::EVRButtonId a_button, const InputCallbackFunc a_callback,
		const Hand hand, const ActionType touch_or_press);

	const float                   GetTrigger(Hand a);
	const vr::VRControllerAxis_t& GetJoystick(Hand a);

	inline Hand GetOtherHand(Hand a)
	{
		return a == Hand::kRight ? Hand::kLeft : (a == Hand::kLeft ? Hand::kRight : Hand::kBoth);
	}

	inline RE::NiAVObject* GetHandNode(Hand a_hand, bool a_first_person)
	{
		return RE::PlayerCharacter::GetSingleton()
			->Get3D(a_first_person)
			->GetObjectByName(a_hand == Hand::kRight ? kRightHandNodeName : kLeftHandNodeName);
	}

	bool ControllerInputCallback(vr::TrackedDeviceIndex_t unControllerDeviceIndex,
		const vr::VRControllerState_t* pControllerState, uint32_t unControllerStateSize,
		vr::VRControllerState_t* pOutputControllerState);

	vr::EVRCompositorError ControllerPoseCallback(VR_ARRAY_COUNT(unRenderPoseArrayCount)
													  vr::TrackedDevicePose_t* pRenderPoseArray,
		uint32_t                                                      unRenderPoseArrayCount,
		VR_ARRAY_COUNT(unGamePoseArrayCount) vr::TrackedDevicePose_t* pGamePoseArray,
		uint32_t                                                      unGamePoseArrayCount);
}
