#include "server.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"

CoffeeServer::CoffeeServer(Scale &scale) : scale(scale)
{
}

void CoffeeServer::setup(Modes::Mode *mode)
{
    currentMode = mode;

    server.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");
    server.begin();

    apiServer.on("/api/info", [this](AsyncWebServerRequest *request)
                 {
                     auto start = millis();

                     Serial.printf("ram free before alloc: %d\n", ESP.getMaxFreeBlockSize());

                     const auto size = 14000;
                     if (ESP.getMaxFreeBlockSize() < size + 2048) {
                         Serial.println("ram too full, don't respond");
                         request->send(500);
                         return;
                     }

                     AsyncJsonResponse *response = new AsyncJsonResponse(false, 14000);

                     Serial.printf("ram free after alloc: %d\n", ESP.getMaxFreeBlockSize());

                     auto doc = response->getRoot();
                     doc["currentMode"] = currentMode->getID();
                     doc["currentTime"] = millis();
                     doc["currentWeight"] = scale.lastWeight;

                     JsonObject mode = doc.createNestedObject("mode");
                     currentMode->createJSONSummary(mode);

                     JsonArray data = doc.createNestedArray("history");
                     for (size_t i = 0; i < scale.history.size(); i++)
                     {
                         auto entry = scale.history[i];
                         data.add(entry.timestamp);
                         data.add(static_cast<int>(entry.weight * 100));
                     }

                     Serial.printf("json size: %d\n", doc.memoryUsage());

                     response->setLength();
                     response->addHeader("Access-Control-Allow-Origin", "*");
                     request->send(response);

                     Serial.printf("/api/info %dms\n", millis() - start);
                 });
    apiServer.begin();
}
