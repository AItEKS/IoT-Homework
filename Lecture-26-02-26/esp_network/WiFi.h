#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;

String id(){
    //TODO make it from MAC
    uint8_t mac[WL_MAC_ADDR_LENGTH];
    WiFi.softAPmacAddress(mac);
    String macID = String(mac[WL_MAC_ADDR_LENGTH-2], HEX) + String(mac[WL_MAC_ADDR_LENGTH-1], HEX);
    return macID;
}

void start_AP_mode() {
    IPAddress AP_IP(192, 168, 4, 1);
    Serial.println("Attempt to start WiFi AP");
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(SSID_AP + "_" + id(), PASSWORD_AP);
    Serial.println("WiFi AP is up, see " + SSID_AP + "_" + id());
}

void start_client_mode() {
    Serial.print("Attempt to set client to ssid: ");
    Serial.println(SSID_CLI);
    wifiMulti.addAP(SSID_CLI.c_str(), SSID_PASSWORD.c_str());
    while(wifiMulti.run() != WL_CONNECTED) {
        Serial.print("Successfuly connected to router");
    }
}

void init_WiFi(bool mode) {
    start_AP_mode();
    return;
    if (mode == WIFI_MODE_CLIENT) {
        start_client_mode();
        String ip = WiFi.localIP().toString();
        Serial.print("My IP address is:");
        Serial.println(ip);
    } else {
        start_AP_mode();
        String ip = WiFi.localIP().toString();
        Serial.print("My IP address is:");
        Serial.println(ip);
    }
}