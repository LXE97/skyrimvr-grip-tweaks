#include "equip_manager.h"

#include "config.h"

namespace equip_manager
{
    using namespace RE;
    using namespace vrinput;

    BGSEquipSlot* g_twohandequip;
    BGSEquipSlot* g_onehandequip;
    BGSEquipSlot* g_shieldequip;

    void Init()
    {
        g_twohandequip = TESForm::LookupByID(kTwoHandEquipSlot)->As<BGSEquipSlot>();
        g_onehandequip = TESForm::LookupByID(kRightHandEquipSlot)->As<BGSEquipSlot>();
        g_shieldequip = TESForm::LookupByID(kShieldEquipSlot)->As<BGSEquipSlot>();

        auto equip_sink = EventSink<TESEquipEvent>::GetSingleton();
        ScriptEventSourceHolder::GetSingleton()->AddEventSink(equip_sink);
        equip_sink->AddCallback(EquippedEventHandler);

        vrinput::AddCallback(vr::k_EButton_SteamVR_Trigger, AttackButtonHandler,
            (int)Config::GetSingleton()->GetSetting(Config::LeftHandedMode) ? Hand::kRight :
                                                                               Hand::kLeft,
            ActionType::kPress);
    }

    // Reset weapon slots to normal
    void EquippedEventHandler(const TESEquipEvent* event)
    {
        auto right = PlayerCharacter::GetSingleton()->GetEquippedObject(false);
        auto left = PlayerCharacter::GetSingleton()->GetEquippedObject(true);
        if (event->actor.get() == PlayerCharacter::GetSingleton())
        {
            if (auto form = TESForm::LookupByID(event->baseObject);
                form && form->GetFormType() == FormType::Weapon ||
                form->GetFormType() == FormType::Spell ||
                (form->GetFormType() == FormType::Armor &&
                    form->As<TESObjectARMO>()->GetEquipSlot() == g_shieldequip))
            {
                // reset the currently equipped weapons
                if (event->equipped)
                {
                    FixEquipSlot(PlayerCharacter::GetSingleton()->GetEquippedObject(false), false);
                    FixEquipSlot(PlayerCharacter::GetSingleton()->GetEquippedObject(true), false);
                }
                else
                {  // reset the outgoing weapon
                    FixEquipSlot(form, false);
                }
            }
        }
    }

    // Simulate off hand casting only
    bool AttackButtonHandler(const ModInputEvent& e)
    {
        auto        poop = Config::GetSingleton();
        static bool g_casting_state = false;
        if (e.button_state == ButtonState::kButtonDown && !g_casting_state)
        {
            if (IsTweakWeapon(PlayerCharacter::GetSingleton()->GetEquippedObject(false)))
            {
                if (auto offhand_form = PlayerCharacter::GetSingleton()->GetEquippedObject(true);
                    offhand_form && offhand_form->GetFormType() == FormType::Spell)
                {
                    FocusWindow();
                    SendClick(true, false);
                    g_casting_state = true;
                    return true;
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

    // Set all weapons to one handed if applicable to avoid unnecessary unequipping
    void PlayerEquipHook(RE::TESForm* a_form)
    {
        if (auto type = a_form->GetFormType(); type == FormType::Weapon ||
            (type == FormType::Armor &&
                a_form->As<TESObjectARMO>()->GetEquipSlot() == g_shieldequip))
        {
            FixEquipSlot(PlayerCharacter::GetSingleton()->GetEquippedObject(false), true);
            FixEquipSlot(PlayerCharacter::GetSingleton()->GetEquippedObject(true), true);
            FixEquipSlot(a_form, true);
        }
        else if (type == FormType::Spell)
        {
            FixEquipSlot(PlayerCharacter::GetSingleton()->GetEquippedObject(false), true);
            FixEquipSlot(PlayerCharacter::GetSingleton()->GetEquippedObject(true), true);
        }
    }

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

    bool IsTweakWeapon(RE::TESForm* a_form)
    {
        if (a_form)
        {
            if (auto weap = a_form->As<TESObjectWEAP>())
            {
                return (weapontypes_to_modify.contains(weap->GetWeaponType()));
            }
        }
        return false;
    }

}
