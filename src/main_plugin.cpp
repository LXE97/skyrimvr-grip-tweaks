#include "main_plugin.h"

namespace griptweaks
{
    using namespace vrinput;
    using namespace RE;

    OpenVRHookManagerAPI* g_OVRHookManager;

    void StartMod()
    {
        RegisterVRInputCallback();
        menu_checker::Init();
		Config::GetSingleton()->Load();

        if (Config::GetSingleton()->GetSetting(Config::Enable2Hto1H) && g_higgsInterface)
        {
            hooks::Install();
            hooks::InstallHIGGS();
            equip_manager::Init();
            SKSE::log::info("Enabled feature: 2H to 1H");
        }

        if (Config::GetSingleton()->GetSetting(Config::EnableHiggsGripMemory) && g_higgsInterface) {
			SKSE::log::info("Enabled feature: HIGGS Grip Memory");
		}
        if (Config::GetSingleton()->GetSetting(Config::EnableGripControlConditions)) {
			SKSE::log::info("Enabled feature: HIGGS/VRIK Conditions");
		}
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
