#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "th3_config.h"

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

const int ledPin = LED_PIN;
String clientId;
int counter = 0;
unsigned long lastPublishMs = 0;
unsigned long lastReconnectMs = 0;

void connectWifi() {
  Serial.print("Dang ket noi Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    tries++;
    if (tries % 40 == 0) {
      Serial.println();
      Serial.println("Chua vao duoc mang, kiem tra WIFI_SSID / WIFI_PASSWORD trong common/wifi_secrets.h");
    }
  }
  Serial.println();
  Serial.print("Da vao mang, IP: ");
  Serial.println(WiFi.localIP());
}

void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("[MQTT] Nhan <- ");
  Serial.print(topic);
  Serial.print(" : ");
  Serial.println(message);

  if (String(topic) != MQTT_TOPIC_LED) {
    return;
  }
  if (message == "1") {
    digitalWrite(ledPin, HIGH);
    Serial.printf("LED GPIO%d: BAT\n", ledPin);
  } else if (message == "0") {
    digitalWrite(ledPin, LOW);
    Serial.printf("LED GPIO%d: TAT\n", ledPin);
  } else {
    Serial.println("Lenh khong hop le, chi nhan '1' hoac '0'");
  }
}

bool connectMqtt() {
  Serial.print("[MQTT] Ket noi broker ");
  Serial.print(MQTT_BROKER);
  Serial.print(":");
  Serial.print(MQTT_PORT);
  Serial.print(", client id = ");
  Serial.println(clientId);

  if (!mqtt.connect(clientId.c_str())) {
    Serial.print("[MQTT] That bai, state = ");
    Serial.print(mqtt.state());
    Serial.println(", thu lai sau 5 giay");
    return false;
  }

  Serial.println("[MQTT] Da ket noi broker");
  mqtt.subscribe(MQTT_TOPIC_LED);
  Serial.print("[MQTT] Da subscribe: ");
  Serial.println(MQTT_TOPIC_LED);
  return true;
}

void publishCounter() {
  String payload = String(counter++);
  bool ok = mqtt.publish(MQTT_TOPIC_COUNTER, payload.c_str());
  Serial.print("[MQTT] Gui -> ");
  Serial.print(MQTT_TOPIC_COUNTER);
  Serial.print(" : ");
  Serial.print(payload);
  Serial.println(ok ? "" : " (loi)");
}

void sendLedCommandFromSerial() {
  if (!Serial.available()) {
    return;
  }
  char key = Serial.read();
  if (key != '0' && key != '1') {
    return;
  }
  char command[2] = {key, '\0'};
  mqtt.publish(MQTT_TOPIC_LED, command);
  Serial.print("[MQTT] Gui lenh -> ");
  Serial.print(MQTT_TOPIC_LED);
  Serial.print(" : ");
  Serial.println(command);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("Bai 5 - MQTT publish/subscribe");

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  connectWifi();

  String mac = WiFi.macAddress();
  mac.replace(":", "");
  clientId = "esp32-th3-" + mac.substring(6);

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(onMqttMessage);
  connectMqtt();
  lastReconnectMs = millis();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Mat Wi-Fi, ket noi lai");
    connectWifi();
  }

  if (!mqtt.connected()) {
    if (millis() - lastReconnectMs > 5000) {
      lastReconnectMs = millis();
      connectMqtt();
    }
    return;
  }

  mqtt.loop();
  sendLedCommandFromSerial();

  if (millis() - lastPublishMs >= MQTT_PUBLISH_INTERVAL_MS) {
    lastPublishMs = millis();
    publishCounter();
  }
}
