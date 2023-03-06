inline void wifi_setup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  ArduinoOTA.setPassword(ota_password);
  ArduinoOTA.begin();
}

inline void wifi_loop() {
  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.handle();
  }
}
