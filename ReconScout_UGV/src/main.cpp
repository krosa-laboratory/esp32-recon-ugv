#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>


// Access Points credentials
const char* ssid = "ReconScout_UGV";
const char* pswd = "ReConScouT_001";

// Servers initialization (80 for HTTP, and 81 for WebSockets)
AsyncWebServer Server(80);
WebSocketsServer WebSocket = WebSocketsServer(81);

// Global variables to store motor speeds
int target_speed_left = 0;
int target_speed_right = 0;


void webSocketEventHandler(uint8_t number, WStype_t type, uint8_t* payload, size_t length) {

    if (type == WStype_TEXT) { // Filter messages

        String data = (char*)payload; // Convert raw data to readable text
        int comma_index = data.indexOf(','); // Search for the delimitter (date ej. "100,-50")

        if (comma_index > 0) {

            // Extract the values and convert them to integer
            target_speed_left = data.substring(0, comma_index).toInt();
            target_speed_right = data.substring(comma_index + 1).toInt();

            Serial.printf(
                "- Received command -> L motor: %d | R motor: %d\n",
                target_speed_left, target_speed_right
            );

        }

    }

}


void setup() {

    Serial.begin(115200);
    Serial.print("--- Starting UGV...");

    WiFi.softAP(ssid, pswd);

    IPAddress IP = WiFi.softAPIP();
    Serial.print(" Access Point created. UGV IP:");
    Serial.println(IP);

    WebSocket.begin();
    WebSocket.onEvent(webSocketEventHandler);
    Serial.println("--- Web Socket started in port 81, waiting commands...");

}


void loop() {

    WebSocket.loop();

}
