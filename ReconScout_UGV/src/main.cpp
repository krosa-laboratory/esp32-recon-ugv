#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

const char* ssid = "ReconScout_UGV";
const char* pswd = "ReConScouT_001";

AsyncWebServer Server(80);
WebSocketsServer WebSocket = WebSocketsServer(81);

void setup() {

    Serial.begin(115200);
    Serial.print("--- Starting UGV...");

    WiFi.softAP(ssid, pswd);

    IPAddress IP = WiFi.softAPIP();
    Serial.print(" Access Point created. UGV IP:");
    Serial.println(IP);

}

void loop() {

    WebSocket.loop();

}
