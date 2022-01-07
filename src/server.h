#include <ESPAsyncWebServer.h>

class CoffeeServer
{
    public:
        CoffeeServer();
        void setup();
    private:
        AsyncWebServer server = AsyncWebServer(80);
};