#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "th3_config.h"
#include "espnow_packet.h"

volatile uint32_t receivedCount = 0;
unsigned long lastHintMs = 0;

void printMac(const uint8_t* mac) {
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void printOwnMac() {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC Receiver: ");
  printMac(mac);
  Serial.println();
  Serial.printf("Dien vao RECEIVER_MAC: {0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X}\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void onDataRecv(const uint8_t* mac, const uint8_t* data, int len) {
  if (len != sizeof(CounterPacket)) {
    Serial.printf("[RX] Bo qua goi %d byte (can %u byte)\n", len, (unsigned)sizeof(CounterPacket));
    return;
  }

  CounterPacket packet;
  memcpy(&packet, data, sizeof(packet));
  receivedCount++;

  Serial.print("[RX] Nhan tu ");
  printMac(mac);
  Serial.printf(": counter = %ld\n", (long)packet.counter);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("Bai 3 - ESP-NOW Receiver");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

  printOwnMac();
  Serial.print("Kenh Wi-Fi: ");
  Serial.println(ESPNOW_CHANNEL);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Loi esp_now_init");
    return;
  }
  esp_now_register_recv_cb(onDataRecv);
  Serial.println("Dang cho goi tin...");
}

void loop() {
  if (receivedCount == 0 && millis() - lastHintMs > 5000) {
    lastHintMs = millis();
    printOwnMac();
  }
  delay(100);
}
