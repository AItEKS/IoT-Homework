#include "Config.h"
#include "WiFi.h"

void setup() {
    Serial.begin(115200);
    init_WiFi(WIFI_START_MODE_AP);
}

void loop() {
    delay(500);
    Serial.print("Our ID: ");
    Serial.println(id());
}