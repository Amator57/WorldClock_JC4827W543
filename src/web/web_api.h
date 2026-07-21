#pragma once

#include <ESPAsyncWebServer.h>

//============================================================
// REST API
//============================================================

// Реєстрація всіх REST API маршрутів
void webApiInit(AsyncWebServer &server);