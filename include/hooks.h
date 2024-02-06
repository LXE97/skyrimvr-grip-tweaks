#pragma once
#include "equip_manager.h"

namespace hooks
{
	using namespace RE;
	using namespace equip_manager;

	void Install();
	void InstallHIGGS();

	struct ActorEquipManager_EquipObject
	{
		static void thunk(ActorEquipManager* a_self, Actor* a_actor, TESBoundObject* a_object,
			ExtraDataList* a_extraData, std::int32_t a_count, const BGSEquipSlot* a_slot,
			bool a_queueEquip, bool a_forceEquip, bool a_playSounds, bool a_applyNow)
		{
			if (a_actor == PlayerCharacter::GetSingleton())
			{
				equip_manager::PlayerEquipHook(a_object);
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
				equip_manager::PlayerEquipHook(a_object);
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
				equip_manager::PlayerEquipHook(a_spell);
			}
			func(a_self, a_actor, a_spell, a_slot);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};
}
