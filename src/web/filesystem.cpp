#include "filesystem.h"

#include <LittleFS.h>

bool filesystemInit()
{
    if (!LittleFS.begin(true))
    {
        Serial.println("LittleFS mount failed.");

        return false;
    }

    Serial.println("LittleFS mounted.");

    File root = LittleFS.open("/");

    File file = root.openNextFile();

    Serial.println("LittleFS files:");

    while (file)
    {
        Serial.print("  ");
        Serial.println(file.name());

        file = root.openNextFile();
    }

    return true;
}