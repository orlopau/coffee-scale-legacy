#include "server.h"

CoffeeServer::CoffeeServer()
{
}

void CoffeeServer::setup()
{
    server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");
    server.begin();
}
