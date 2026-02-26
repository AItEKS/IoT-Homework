#include <ESP8266WiFi.h>
#include <ESPWiFiClient.h>
#include <ESP8266WiFiMulti.h>

String id() {
    uint8_t mac[WL_MAC_ADDR_LENGTH]
    WiFi.softAPmacAddress(mac);
    String macID = String(mac[WL_MAC_ADDR_LENGTH-2], HEX); 
    macID = macID + String(mac[WL_MAC_ADDR_LENGTH-1], HEX)   
    return macID;
}

void start_AP_mode() {
    
}

void start_client_mode() {
    
}

void init_WiFi(bool mode) {
    if (mode == WIFI_MODE_CLIENT) {
        start_client_mode();
    } else {
        start_AP_mode();
    }
}