#pragma once
#include <stdint.h>

#define SETTINGS_SIZE 2

namespace storage
{
    struct SettingsEntry
    {
        const char *name;
        float value;
        const float default_value;
        const float increment;
    };

    struct Network
    {
        uint32_t ip;
        uint32_t gateway;
        uint32_t subnet;
    };

    enum Setting
    {
        SETTING_REG_TARGET_WEIGHT,
        SETTING_REG_OFFSET,
    };

    struct Storage
    {
        float cal_val;
        SettingsEntry entries[SETTINGS_SIZE] = {
            {"REG: target weight", 0, 37, 1},
            {"REG: offset", 0, 2, 0.1},
        };
        Network network;
    };

    extern Storage data;
    void updateData();
    void saveData();
}