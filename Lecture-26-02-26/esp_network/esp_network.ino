#include "Config.h"
#include "WiFi.h"
#include "Server.h"

void setup() {
    pinMode(led, OUTPUT);
    Serial.begin(115200);
    init_WiFi(WIFI_MODE_AP);
    init_server();  
}

void loop() {
    server.handleClient();
    //delay(500);
    //Serial.print("Our ID: ");
    //Serial.println(id());
}