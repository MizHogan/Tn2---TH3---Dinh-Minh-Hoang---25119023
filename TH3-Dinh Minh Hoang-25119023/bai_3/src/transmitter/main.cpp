#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "th3_config.h"
#include "espnow_packet.h"

uint8_t receiverMac[] = RECEIVER_MAC;
CounterPacket packet = {0};
bool espNowReady = false;

void printMac(const uint8_t* mac) {
  Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void onDataSent(const uint8_t* mac, esp_now_send_status_t status) {
  Serial.print("[TX] Ket qua gui toi ");
  printMac(mac);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? ": THANH CONG (Receiver da ACK)" : ": THAT BAI (khong co ACK)");
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("Bai 3 - ESP-NOW Transmitter");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

  Serial.print("MAC Transmitter: ");
  Serial.println(WiFi.macAddress());
  Serial.print("MAC Receiver (RECEIVER_MAC): ");
  printMac(receiverMac);
  Serial.println();
  Serial.print("Kenh Wi-Fi: ");
  Serial.println(ESPNOW_CHANNEL);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Loi esp_now_init");
    return;
  }
  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, receiverMac, 6);
  peer.channel = ESPNOW_CHANNEL;
  peer.encrypt = false;
  if (esp_now_add_peer(&peer) != ESP_OK) {
    Serial.println("Loi esp_now_add_peer");
    return;
  }
  espNowReady = true;
}

void loop() {
  if (!espNowReady) {
    delay(1000);
    return;
  }

  packet.counter++;
  esp_err_t result = esp_now_send(receiverMac, (uint8_t*)&packet, sizeof(packet));
  Serial.printf("[TX] Gui counter = %ld (%u byte), esp_now_send: %s\n",
                (long)packet.counter, (unsigned)sizeof(packet), esp_err_to_name(result));
  delay(ESPNOW_SEND_INTERVAL_MS);
}
