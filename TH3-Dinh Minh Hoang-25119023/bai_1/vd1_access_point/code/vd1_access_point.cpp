#include <Arduino.h>
#include <WiFi.h>

const char* ssid = "MizHoagn";
const char* pass = "527527527";

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, pass);
  Serial.print("AP SSID: ");
  Serial.println(ssid);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  static int truoc = -1;
  int n = WiFi.softAPgetStationNum();
  if (n != truoc) {
    Serial.printf("So thiet bi dang ket noi: %d\n", n);
    truoc = n;
  }
  delay(500);
}
