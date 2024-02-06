#pragma once
#include "mod_event_sink.hpp"
#include "mod_input.h"

namespace equip_manager
{
	constexpr RE::FormID kTwoHandEquipSlot = 0x00013F45;
	constexpr RE::FormID kRightHandEquipSlot = 0x00013F42;
	constexpr RE::FormID kShieldEquipSlot = 0x000141E8;

	extern RE::BGSEquipSlot* g_twohandequip;
	extern RE::BGSEquipSlot* g_onehandequip;
	extern RE::BGSEquipSlot* g_shieldequip;

	const std::set<RE::WeaponTypes::WEAPON_TYPE> weapontypes_to_modify = {
		RE::WeaponTypes::kTwoHandAxe, RE::WeaponTypes::kTwoHandSword,
		//RE::WeaponTypes::kCrossbow,
	};

	void EquippedEventHandler(const RE::TESEquipEvent* event);
	bool AttackButtonHandler(const vrinput::ModInputEvent& e);
	void PlayerEquipHook(RE::TESForm* a_form);

	void FixEquipSlot(RE::FormID a_formID, bool a_make_onehanded);
	void FixEquipSlot(RE::TESForm* a_form, bool a_make_onehanded);

	bool IsTweakWeapon(RE::TESForm* a_form);

	void Init();
}
