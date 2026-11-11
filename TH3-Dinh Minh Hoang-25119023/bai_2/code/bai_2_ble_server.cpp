#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#define TEN_BLE "MizHoang"
#define SV_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CH_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *sv) override {
    Serial.println("Dien thoai da ket noi");
  }
  void onDisconnect(BLEServer *sv) override {
    Serial.println("Dien thoai ngat ket noi -> quang ba lai");
    BLEDevice::startAdvertising();
  }
};

class CharCallbacks : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *ch) override {
    Serial.printf("Dien thoai DOC: %s\n", ch->getValue().c_str());
  }
  void onWrite(BLECharacteristic *ch) override {
    Serial.printf("Dien thoai GHI gia tri moi: %s\n", ch->getValue().c_str());
  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init(TEN_BLE);
  auto *sv = BLEDevice::createServer();
  sv->setCallbacks(new ServerCallbacks());
  auto *se = sv->createService(SV_UUID);
  auto *ch = se->createCharacteristic(CH_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  ch->setCallbacks(new CharCallbacks());
  ch->setValue("MSSV 25119023");
  se->start();
  BLEDevice::getAdvertising()->addServiceUUID(SV_UUID);
  BLEDevice::getAdvertising()->start();
  Serial.printf("BLE server '%s' dang quang ba, gia tri: %s\n", TEN_BLE, ch->getValue().c_str());
}

void loop() { delay(2000); }
