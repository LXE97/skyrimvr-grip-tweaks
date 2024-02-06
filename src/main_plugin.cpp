#include "main_plugin.h"

namespace griptweaks
{
	using namespace vrinput;
	using namespace RE;

	OpenVRHookManagerAPI* g_OVRHookManager;

	// TODO: ini setting
	bool g_left_handed_mode = false;

	void StartMod()
	{
		RegisterVRInputCallback();
		hooks::Install();
		if (g_higgsInterface) { hooks::InstallHIGGS(); }
		menu_checker::Init();
		equip_manager::Init();

		// TODO: ini setting for this button
		vrinput::AddCallback(vr::k_EButton_SteamVR_Trigger, equip_manager::AttackButtonHandler,
			g_left_handed_mode ? Hand::kRight : Hand::kLeft, ActionType::kPress);
	}

	// 2. Set / reset grip memory button handler
	bool GripMemoryButtonHandler(const ModInputEvent& e) { return false; }

	// 2. Set grip location to memory
	bool OnHiggsGrip() { return false; }

	void RegisterVRInputCallback()
	{
		if (g_OVRHookManager->IsInitialized())
		{
			g_OVRHookManager = RequestOpenVRHookManagerObject();
			if (g_OVRHookManager)
			{
				SKSE::log::info("Successfully requested OpenVRHookManagerAPI.");

				vrinput::g_leftcontroller =
					g_OVRHookManager->GetVRSystem()->GetTrackedDeviceIndexForControllerRole(
						vr::TrackedControllerRole_LeftHand);
				vrinput::g_rightcontroller =
					g_OVRHookManager->GetVRSystem()->GetTrackedDeviceIndexForControllerRole(
						vr::TrackedControllerRole_RightHand);

				g_OVRHookManager->RegisterControllerStateCB(vrinput::ControllerInputCallback);
				g_OVRHookManager->RegisterGetPosesCB(vrinput::ControllerPoseCallback);
			}
		}
	}

}
