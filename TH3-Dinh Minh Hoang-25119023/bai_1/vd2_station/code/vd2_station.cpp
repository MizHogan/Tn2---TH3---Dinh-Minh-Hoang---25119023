#include <Arduino.h>
#include <WiFi.h>

const char* ssid = "MizHoang";
const char* pass = "527527527";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  Serial.printf("Dang noi vao %s", ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nIP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
}

void loop() {}
