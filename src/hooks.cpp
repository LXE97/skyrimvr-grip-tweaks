#pragma once
#include "hooks.h"

namespace hooks
{
	using namespace RE;

	void Install()
	{
		REL::Relocation<std::uintptr_t> ActorEquipManager_EquipObject_target{
			Offset::ActorEquipManager::EquipObject, 0xE5
		};
		stl::write_thunk_call<ActorEquipManager_EquipObject>(
			ActorEquipManager_EquipObject_target.address());

		REL::Relocation<std::uintptr_t> ActorEquipManager_UnequipObject_target{
			Offset::ActorEquipManager::UnequipObject, 0x138
		};
		stl::write_thunk_call<ActorEquipManager_UnequipObject>(
			ActorEquipManager_UnequipObject_target.address());

		REL::Relocation<std::uintptr_t> ActorEquipManager_EquipSpell_target{ REL::Offset(
			0x641A30 + 0xD7) };
		stl::write_thunk_call<ActorEquipManager_EquipSpell>(
			ActorEquipManager_EquipSpell_target.address());
	}

	void InstallHIGGS()
	{
		// Wipe out the checks that make two-handed weapons always grabbable
		REL::Relocation<std::uintptr_t>        Hand_CanTwoHand_offset{ 0x00025a10 };
		std::unordered_map<uint16_t, uint64_t> offset_data{
			{ 0xA7, 0xE983909090909090 },
			{ 0xB0, 0xF983909090909090 },
			{ 0xB9, 0xFA80909090909090 },
			{ 0x1B3, 0xE983909090909090 },
			{ 0x1BC, 0xF983909090909090 },
			{ 0x1C5, 0xFA80909090909090 },
		};

		if (HMODULE hModule = GetModuleHandle(L"higgs_vr.dll"))
		{
			for (auto& [offset, data] : offset_data)
			{
				REL::safe_write(
					(std::uintptr_t)hModule + Hand_CanTwoHand_offset.address() + offset, data);
			}
		}
	}
}
