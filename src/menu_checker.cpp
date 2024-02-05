#include "menu_checker.h"

#include "mod_event_sink.hpp"

namespace menu_checker
{
	bool game_stopped_state = true;

	bool IsGameStopped() { return game_stopped_state; }

	bool UpdateGameStopped()
	{
		for (auto menu : gamestopping_menus)
		{
			if (menu_types[menu] == true) { return true; }
		}
		return false;
	}

	void OnMenuOpenClose(RE::MenuOpenCloseEvent const* evn)
	{
		const char* menu_name = evn->menuName.data();

		if (evn->opening)
		{  // Menu opened
			if (menu_types.find(menu_name) != menu_types.end()) { menu_types[menu_name] = true; }
		}
		else
		{  // Menu closed
			if (menu_types.find(menu_name) != menu_types.end()) { menu_types[menu_name] = false; }
		}

		game_stopped_state = UpdateGameStopped();
	}

	void Init()
	{
		std::once_flag once;
		std::call_once(once, []() {
			auto menu_sink = EventSink<RE::MenuOpenCloseEvent>::GetSingleton();
			menu_sink->AddCallback(OnMenuOpenClose);
			RE::UI::GetSingleton()->AddEventSink(menu_sink);
		});
	}

	std::vector<std::string> gamestopping_menus{ "BarterMenu", "Book Menu", "Console",
		"Native UI Menu", "ContainerMenu", "Dialogue Menu", "Crafting Menu", "Credits Menu",
		"Cursor Menu", "Debug Text Menu", "FavoritesMenu", "GiftMenu", "InventoryMenu",
		"Journal Menu", "Kinect Menu", "Loading Menu", "Lockpicking Menu", "MagicMenu", "Main Menu",
		"MapMarkerText3D", "MapMenu", "MessageBoxMenu", "Mist Menu", "Quantity Menu",
		"RaceSex Menu", "Sleep/Wait Menu", "StatsMenuSkillRing", "StatsMenuPerks", "Training Menu",
		"Tutorial Menu", "TweenMenu" };

	std::unordered_map<std::string, bool> menu_types(
		{ { "BarterMenu", false }, { "Book Menu", false }, { "Console", false },
			{ "Native UI Menu", false }, { "ContainerMenu", false }, { "Dialogue Menu", false },
			{ "Crafting Menu", false }, { "Credits Menu", false }, { "Cursor Menu", false },
			{ "Debug Text Menu", false }, { "Fader Menu", false }, { "FavoritesMenu", false },
			{ "GiftMenu", false }, { "HUD Menu", false }, { "InventoryMenu", false },
			{ "Journal Menu", false }, { "Kinect Menu", false }, { "Loading Menu", false },
			{ "Lockpicking Menu", false }, { "MagicMenu", false }, { "Main Menu", false },
			{ "MapMarkerText3D", false }, { "MapMenu", false }, { "MessageBoxMenu", false },
			{ "Mist Menu", false }, { "Overlay Interaction Menu", false },
			{ "Overlay Menu", false }, { "Quantity Menu", false }, { "RaceSex Menu", false },
			{ "Sleep/Wait Menu", false }, { "StatsMenu", false }, { "StatsMenuPerks", false },
			{ "StatsMenuSkillRing", false }, { "TitleSequence Menu", false }, { "Top Menu", false },
			{ "Training Menu", false }, { "Tutorial Menu", false }, { "TweenMenu", false },
			{ "WSEnemyMeters", false }, { "WSDebugOverlay", false },
			{ "WSActivateRollover", false }, { "LoadWaitSpinner", false } });

}
