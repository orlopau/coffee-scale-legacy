#include <ESPAsyncWebServer.h>
#include <scale.h>
#include "modes.h"

class CoffeeServer
{
public:
    CoffeeServer(Scale &scale);
    void setup(Modes::Mode *mode);
    Modes::Mode *currentMode;

private:
    Scale &scale;
    AsyncWebServer server = AsyncWebServer(80);
    AsyncWebServer apiServer = AsyncWebServer(1122);
};