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
		menu_checker::Init();
		equip_manager::Init();

		// TODO: ini setting for this button
		vrinput::AddCallback(vr::k_EButton_SteamVR_Trigger, AttackButtonHandler,
			g_left_handed_mode ? Hand::kRight : Hand::kLeft, ActionType::kPress);

		auto equip_sink = EventSink<TESEquipEvent>::GetSingleton();
		ScriptEventSourceHolder::GetSingleton()->AddEventSink(equip_sink);
		equip_sink->AddCallback(EquipEventHandler);
	}

	// 1. Simulate off hand casting	only
	bool AttackButtonHandler(const ModInputEvent& e)
	{
		static bool g_casting_state = false;
		if (e.button_state == ButtonState::kButtonDown && !g_casting_state)
		{
			if (equip_manager::IsTweakWeapon(
					PlayerCharacter::GetSingleton()->GetEquippedObject(false)))
			{
				if (auto offhand_form = PlayerCharacter::GetSingleton()->GetEquippedObject(true))
				{
					if (offhand_form->GetFormType() == FormType::Spell)
					{
						FocusWindow();
						SendClick(true, false);
						g_casting_state = true;
						return true;
					}
				}
			}
		}
		else if (e.button_state == ButtonState::kButtonUp && g_casting_state)
		{
			FocusWindow();
			SendClick(false, false);
			g_casting_state = false;
		}
		return false;
	}

	// 1. Reset weapon slots to normal
	void EquipEventHandler(const TESEquipEvent* event)
	{
		auto right = PlayerCharacter::GetSingleton()->GetEquippedObject(false);
		auto left = PlayerCharacter::GetSingleton()->GetEquippedObject(true);
		if (event->actor.get() == PlayerCharacter::GetSingleton())
		{
			if (auto form = TESForm::LookupByID(event->baseObject);
				form->GetFormType() == FormType::Weapon || form->GetFormType() == FormType::Spell ||
				(form->GetFormType() == FormType::Armor &&
					form->As<TESObjectARMO>()->GetEquipSlot() == equip_manager::g_shieldequip))
			{
				// reset the currently equipped weapons
				if (event->equipped)
				{
					equip_manager::FixEquipSlot(
						PlayerCharacter::GetSingleton()->GetEquippedObject(false), false);
					equip_manager::FixEquipSlot(
						PlayerCharacter::GetSingleton()->GetEquippedObject(true), false);
				}
				else
				{  // reset the outgoing weapon
					equip_manager::FixEquipSlot(form, false);
				}
			}
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
