#pragma once
#include "equip_manager.h"
#include "menu_checker.h"
#include "mod_input.h"

namespace hooks
{
	using namespace RE;
	using namespace equip_manager;

	struct ActorEquipManager_EquipObject
	{
		static void thunk(ActorEquipManager* a_self, Actor* a_actor, TESBoundObject* a_object,
			ExtraDataList* a_extraData, std::int32_t a_count, const BGSEquipSlot* a_slot,
			bool a_queueEquip, bool a_forceEquip, bool a_playSounds, bool a_applyNow)
		{
			if (a_actor == PlayerCharacter::GetSingleton())
			{
				if (auto type = a_object->GetFormType(); type == FormType::Weapon ||
					(type == FormType::Armor &&
						a_object->As<TESObjectARMO>()->GetEquipSlot() == g_shieldequip))
				{  // set all weapons to one handed if applicable to avoid unnecessary unequipping
					FixEquipSlot(PlayerCharacter::GetSingleton()->GetEquippedObject(false), true);
					FixEquipSlot(PlayerCharacter::GetSingleton()->GetEquippedObject(true), true);
					FixEquipSlot(a_object, true);
				}
			}
			func(a_self, a_actor, a_object, a_extraData, a_count, a_slot, a_queueEquip,
				a_forceEquip, a_playSounds, a_applyNow);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct ActorEquipManager_UnequipObject
	{
		static void thunk(ActorEquipManager* a_self, Actor* a_actor, TESBoundObject* a_object,
			ExtraDataList* a_extraData, std::int32_t a_count, const BGSEquipSlot* a_slot,
			bool a_queueEquip, bool a_forceEquip, bool a_playSounds, bool a_applyNow,
			const BGSEquipSlot* a_slotToReplace = nullptr)
		{
			if (a_actor == PlayerCharacter::GetSingleton())
			{
				if (auto type = a_object->GetFormType(); type == FormType::Weapon ||
					(type == FormType::Armor &&
						a_object->As<TESObjectARMO>()->GetEquipSlot() == g_shieldequip))
				{  // set all weapons to one handed if applicable to avoid unnecessary unequipping
					FixEquipSlot(PlayerCharacter::GetSingleton()->GetEquippedObject(false), true);
					FixEquipSlot(PlayerCharacter::GetSingleton()->GetEquippedObject(true), true);
					FixEquipSlot(a_object, true);
				}
			}
			func(a_self, a_actor, a_object, a_extraData, a_count, a_slot, a_queueEquip,
				a_forceEquip, a_playSounds, a_applyNow, a_slotToReplace);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct ActorEquipManager_EquipSpell
	{
		static void thunk(ActorEquipManager* a_self, Actor* a_actor, SpellItem* a_spell,
			const BGSEquipSlot* a_slot)
		{
			if (a_actor == PlayerCharacter::GetSingleton())
			{
				FixEquipSlot(PlayerCharacter::GetSingleton()->GetEquippedObject(false), true);
				FixEquipSlot(PlayerCharacter::GetSingleton()->GetEquippedObject(true), true);
			}

			func(a_self, a_actor, a_spell, a_slot);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install();
	void InstallHIGGS();
}
