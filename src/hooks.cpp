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

		REL::Relocation<std::uintptr_t> ActorEquipManager_EquipSpell_target{ REL::Offset(
			0x641A30 + 0xd7) };
		stl::write_thunk_call<ActorEquipManager_EquipSpell>(
			ActorEquipManager_EquipSpell_target.address());

		stl::write_vfunc<RE::PlayerCharacter, 0x7D, PlayerCharacter_Update>();
	}
}
