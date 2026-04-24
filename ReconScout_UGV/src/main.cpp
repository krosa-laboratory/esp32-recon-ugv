#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include "WebPage.h"

// Motor pins
#define LEFT_MOTOR_IN1 12
#define LEFT_MOTOR_IN2 13
#define RIGHT_MOTOR_IN1 14
#define RIGHT_MOTOR_IN2 15

// PWM channels from ESP32
#define CHANNEL_L1 0
#define CHANNEL_L2 1
#define CHANNEL_R1 2
#define CHANNEL_R2 3

// Access Points credentials
const char* ssid = "ReconScout_UGV";
const char* pswd = "ReConScouT_001";

// Servers initialization (80 for HTTP, and 81 for WebSockets)
AsyncWebServer Server(80);
WebSocketsServer WebSocket = WebSocketsServer(81);

// Global variables to store motor speeds
int target_speed_left = 0;
int target_speed_right = 0;

// Global variable for security break
unsigned long last_command_time = 0;


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

            last_command_time = millis();

        }

    }

}


void setupMotors() {

    // Configure PWM properties
    ledcSetup(CHANNEL_L1, 1000, 8);
    ledcSetup(CHANNEL_L2, 1000, 8);
    ledcSetup(CHANNEL_R1, 1000, 8);
    ledcSetup(CHANNEL_R2, 1000, 8);
    // Link logic channels with GPIOs
    ledcAttachPin(LEFT_MOTOR_IN1, CHANNEL_L1);
    ledcAttachPin(LEFT_MOTOR_IN2, CHANNEL_L2);
    ledcAttachPin(RIGHT_MOTOR_IN1, CHANNEL_R1);
    ledcAttachPin(RIGHT_MOTOR_IN2, CHANNEL_R2);
    // Start with them stopped
    ledcWrite(CHANNEL_L1, 0);
    ledcWrite(CHANNEL_L2, 0);
    ledcWrite(CHANNEL_R1, 0);
    ledcWrite(CHANNEL_R2, 0);

}


void updateMotors() {

    // --- LEFT Motor control
    if (target_speed_left == 0) { // If speed is 0, it must be stopped
        ledcWrite(CHANNEL_L1, 0);
        ledcWrite(CHANNEL_L2, 0);
    } else { // In this case we calculate the PWM value with the speed received

        int left_pwm = map(abs(target_speed_left), 0, 100, 0, 255);
        Serial.printf("-- LEFT PWM adjusted to %d\n", left_pwm);

        if (target_speed_left > 0) {
            ledcWrite(CHANNEL_L1, left_pwm);
            ledcWrite(CHANNEL_L2, 0);
        } else if (target_speed_left < 0) {
            ledcWrite(CHANNEL_L1, 0);
            ledcWrite(CHANNEL_L2, left_pwm);
        }

    }

    // --- RIGHT Motor control
    if (target_speed_right == 0) { // If speed is 0, it must be stopped
        ledcWrite(CHANNEL_R1, 0);
        ledcWrite(CHANNEL_R2, 0);
    } else { // In this case we calculate the PWM value with the speed received

        int right_pwm = map(abs(target_speed_right), 0, 100, 0, 255);
        Serial.printf("-- RIGHT PWM adjusted to %d\n", right_pwm);

        if (target_speed_right > 0) {
            ledcWrite(CHANNEL_R1, right_pwm);
            ledcWrite(CHANNEL_R2, 0);
        } else if (target_speed_right < 0) {
            ledcWrite(CHANNEL_R1, 0);
            ledcWrite(CHANNEL_R2, right_pwm);
        }

    }

}


void setup() {

    Serial.begin(115200);
    Serial.println("--- Starting UGV...");

    setupMotors();

    WiFi.softAP(ssid, pswd);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("--> Access Point created. UGV IP: ");
    Serial.println(IP);

    Server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html);
    });
    Server.begin();
    Serial.println("--> HTTP Server started in port 80. UI ready.");

    WebSocket.begin();
    WebSocket.onEvent(webSocketEventHandler);
    Serial.println("--> Web Socket started in port 81, waiting commands...");

}


void loop() {

    // Maintain the connection openned
    WebSocket.loop();

    // Execute motor commands
    updateMotors();

    // Watchdog if we lose connection
    if (millis() - last_command_time > 200) {
        if (target_speed_left != 0 || target_speed_right != 0) {
            target_speed_left = 0;
            target_speed_right = 0;
            Serial.println("-- CUATION: Connection Lost! UGV Stopped.");
        }
    }

}
