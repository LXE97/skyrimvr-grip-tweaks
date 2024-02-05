#include "main_plugin.h"

namespace griptweaks
{
	using namespace vrinput;
	using namespace RE;

	OpenVRHookManagerAPI* g_OVRHookManager;

	void StartMod()
	{
		hooks::Install();
		menuchecker::Init();
		equipmanager::Init();

		auto equipSink = EventSink<TESEquipEvent>::GetSingleton();
		ScriptEventSourceHolder::GetSingleton()->AddEventSink(equipSink);
		equipSink->AddCallback(onEquipEvent);
	}

	void onEquipEvent(const TESEquipEvent* event)
	{
		if (event->actor.get() == PlayerCharacter::GetSingleton())
		{
			if (auto form = TESForm::LookupByID(event->baseObject);
				form->GetFormType() == FormType::Weapon || form->GetFormType() == FormType::Spell ||
				(form->GetFormType() == FormType::Armor &&
					form->As<TESObjectARMO>()->GetEquipSlot() == hooks::g_shieldequip))
			{
				// reset the incoming/outgoing weapon
				hooks::FixEquipSlot(form, false);

				// reset the currently equipped weapons if we're done with them
				if (event->equipped)
				{
					hooks::FixEquipSlot(
						PlayerCharacter::GetSingleton()->GetEquippedObject(false), false);
					hooks::FixEquipSlot(
						PlayerCharacter::GetSingleton()->GetEquippedObject(true), false);
				}
			}
		}
	}

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