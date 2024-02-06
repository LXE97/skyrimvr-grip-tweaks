#include "config.h"

void Config::Load()
{
    auto error = ini.LoadFile(kIniName);
    if (error) { SKSE::log::warn("ini file bad or not found, defaults will be used"); }
}

float Config::GetSetting(Setting a_setting)
{
    float value = 0.f;
    if (!ini.IsEmpty())
    {
        if (auto val_str = ini.GetValue(kIniSection, setting_str[a_setting]))
        {
            try
            {
                value = std::atof(val_str);
            } catch (std::invalid_argument)
            {
                SKSE::log::warn("ignoring bad ini value");
            } catch (std::out_of_range)
            {
                SKSE::log::warn("ignoring bad ini value");
            }
        }
    } else {
		SKSE::log::warn("failed to get value: ini file not loaded or bad");
	}
    return value;
}

int Config::GetSettingArray(Setting a_setting, int* a_out) { return 0; }
