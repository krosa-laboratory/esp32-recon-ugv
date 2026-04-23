#include <Arduino.h>

#define LED_BUILTIN_PCB 33

void setup() {

    Serial.begin(115200);
    pinMode(LED_BUILTIN_PCB, OUTPUT);
    Serial.println("\n--- UGV Recon Unit: Test ---\nState: ON");

}

void loop() {

    digitalWrite(LED_BUILTIN_PCB, LOW);
    Serial.println("LED ON");
    delay(1000);
    digitalWrite(LED_BUILTIN_PCB, HIGH);
    Serial.println("LED OFF");
    delay(1000);

}
