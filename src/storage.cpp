#include "storage.h"
#include <EEPROM.h>
#include <math.h>
#include <climits>
#include <Arduino.h>

#define START_ADDRESS 0

namespace storage
{
    struct StoredData
    {
        float calVal = 0;
        float setting_values[SETTINGS_SIZE];
    };

    Storage data;

    void updateData()
    {
        StoredData raw;
        EEPROM.get(START_ADDRESS, raw);

        data.cal_val = raw.calVal;

        for (size_t i = 0; i < SETTINGS_SIZE; i++)
        {
            float value = raw.setting_values[i];
            if (isnan(value) || value > 100 || value < 0)
            {
                data.entries[i].value = data.entries[i].default_value;
            }
            else
            {
                data.entries[i].value = value;
            }
        }
    }

    void saveData()
    {
        StoredData raw = {data.cal_val};
        for (size_t i = 0; i < SETTINGS_SIZE; i++)
        {
            raw.setting_values[i] = data.entries[i].value;
        }

        EEPROM.put(START_ADDRESS, raw);
        EEPROM.commit();

        Serial.println("saved to eeprom");
    }
}