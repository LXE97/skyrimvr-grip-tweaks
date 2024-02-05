// Shizof's method
#pragma once

#include <unordered_map>

namespace menu_checker
{
	extern std::vector<std::string> gamestopping_menus;

	extern std::unordered_map<std::string, bool> menu_types;

	bool IsGameStopped();

	void Init();

	void OnMenuOpenClose(RE::MenuOpenCloseEvent const* evn);
}
