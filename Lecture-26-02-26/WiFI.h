String id() {
    // TODO: get MAC-address
    return "";
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