#include "Config.h"
#include "WIFI.h"
#include "Server.h"
#include "MQTT.h"

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  init_WIFI(WIFI_START_MODE_CLIENT);
  init_server();
  init_MQTT();
  mqtt_cli.subscribe("esp8266/command");
}

// the loop function runs over and over again forever
void loop() {
  server.handleClient();
  mqtt_cli.loop();
  //delay(500);
  //Serial.print("Our id is:");
  //Serial.println(id());
}