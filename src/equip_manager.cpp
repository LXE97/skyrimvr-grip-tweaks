#include "equip_manager.h"

namespace equipmanager
{
	using namespace RE;

	BGSEquipSlot* g_twohandequip;
	BGSEquipSlot* g_onehandequip;
	BGSEquipSlot* g_shieldequip;

	void FixEquipSlot(FormID a_formID, bool a_make_onehanded)
	{
		if (auto item = TESForm::LookupByID(a_formID))
		{
			if (auto weap = item->As<TESObjectWEAP>())
			{
				if (weapontypes_to_modify.contains(weap->GetWeaponType()) &&
					(weap->GetEquipSlot() == g_onehandequip != a_make_onehanded))
				{
					weap->SetEquipSlot(a_make_onehanded ? g_onehandequip : g_twohandequip);
				}
			}
		}
	}

	void FixEquipSlot(TESForm* a_form, bool a_make_onehanded)
	{
		if (a_form)
		{
			if (auto weap = a_form->As<TESObjectWEAP>())
			{
				if (weapontypes_to_modify.contains(weap->GetWeaponType()) &&
					(weap->GetEquipSlot() == g_onehandequip != a_make_onehanded))
				{
					weap->SetEquipSlot(a_make_onehanded ? g_onehandequip : g_twohandequip);
				}
			}
		}
	}

	void Init()
	{
		g_twohandequip = TESForm::LookupByID(kTwoHandEquipSlot)->As<BGSEquipSlot>();
		g_onehandequip = TESForm::LookupByID(kRightHandEquipSlot)->As<BGSEquipSlot>();
		g_shieldequip = TESForm::LookupByID(kShieldEquipSlot)->As<BGSEquipSlot>();
	}
}
