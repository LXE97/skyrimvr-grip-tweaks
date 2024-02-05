#include "main_plugin.h"

namespace griptweaks
{
	using namespace vrinput;
	using namespace RE;

	OpenVRHookManagerAPI* g_OVRHookManager;

	bool g_casting_state = false;

	// TODO: ini setting
	bool g_left_handed_mode = false;

	void StartMod()
	{
		RegisterVRInputCallback();
		hooks::Install();
		menu_checker::Init();
		equip_manager::Init();

		// TODO: ini setting for this button
		vrinput::AddCallback(vr::k_EButton_SteamVR_Trigger, OnAttackButton,
			g_left_handed_mode ? Hand::kRight : Hand::kLeft, ActionType::kPress);

		auto equip_sink = EventSink<TESEquipEvent>::GetSingleton();
		ScriptEventSourceHolder::GetSingleton()->AddEventSink(equip_sink);
		equip_sink->AddCallback(OnEquipEvent);

		auto menu_sink = EventSink<RE::MenuOpenCloseEvent>::GetSingleton();
		menu_sink->AddCallback(OnGameMenu);
		RE::UI::GetSingleton()->AddEventSink(menu_sink);
	}

	bool OnAttackButton(const ModInputEvent& e)
	{
		if (e.button_state == ButtonState::kButtonDown && !g_casting_state)
		{  // simulate off hand casting
			if (equip_manager::IsTweakWeapon(
					PlayerCharacter::GetSingleton()->GetEquippedObject(false)))
			{
				if (auto offhand_form = PlayerCharacter::GetSingleton()->GetEquippedObject(true);
					offhand_form->GetFormType() == FormType::Spell)
				{
					SendClick(true, false);
					g_casting_state = true;
					return true;
				}
			}
		}
		else if (e.button_state == ButtonState::kButtonUp && g_casting_state)
		{
			SendClick(false, false);
			g_casting_state = false;
		}
		return false;
	}

	void OnEquipEvent(const TESEquipEvent* event)
	{
		if (event->actor.get() == PlayerCharacter::GetSingleton())
		{
			if (auto form = TESForm::LookupByID(event->baseObject);
				form->GetFormType() == FormType::Weapon || form->GetFormType() == FormType::Spell ||
				(form->GetFormType() == FormType::Armor &&
					form->As<TESObjectARMO>()->GetEquipSlot() == equip_manager::g_shieldequip))
			{
				// reset the incoming/outgoing weapon
				equip_manager::FixEquipSlot(form, false);

				// reset the currently equipped weapons if we're done with them
				if (event->equipped)
				{
					equip_manager::FixEquipSlot(
						PlayerCharacter::GetSingleton()->GetEquippedObject(false), false);
					equip_manager::FixEquipSlot(
						PlayerCharacter::GetSingleton()->GetEquippedObject(true), false);
				}
			}
		}
	}

	void OnGameMenu(RE::MenuOpenCloseEvent const* evn)
	{
		static const BSFixedString gamemenu = "Journal Menu";
		if (!evn->opening && evn->menuName == gamemenu)
		{
			if (auto setting =
					INISettingCollection::GetSingleton()->GetSetting("bLeftHandedMode:VRInput"))
			{
				SKSE::log::trace("got setting");
				g_left_handed_mode = setting->GetBool();
			}
			else { SKSE::log::trace("failed"); }
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
