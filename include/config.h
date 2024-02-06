#pragma once
#include "SimpleIni.h"

class Config
{
public:
    static Config* GetSingleton()
    {
        static Config singleton;
        return &singleton;
    }

    enum Setting
    {
#define X(a) a,
#include "config.def"
#undef X
    };

    void  Load();
    float GetSetting(Setting a_setting);
    int   GetSettingArray(Setting a_setting, int* a_out);

private:
    const wchar_t*    kIniName = L"Data/SKSE/Plugins/vr-grip-tweaks.ini";
    const char* const kIniSection = "griptweaks";
    Config() = default;
    ~Config() = default;
    Config(const Config&) = delete;
    Config(Config&&) = delete;
    Config& operator=(const Config&) = delete;
    Config& operator=(Config&&) = delete;

    std::vector<const char*> setting_str = {
#define X(a) #a,
#include "config.def"
#undef X
        ""
    };

    CSimpleIniA ini;
};
