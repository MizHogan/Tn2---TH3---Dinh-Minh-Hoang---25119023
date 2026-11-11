#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "TEN_WIFI";
const char* pass = "MAT_KHAU";

WebServer server(80);
const int LED = 2;
bool on = false;

void datLed(bool trangThai) {
  on = trangThai;
  digitalWrite(LED, on);
  Serial.printf("LED -> %s\n", on ? "BAT" : "TAT");
}

void handleRoot() {
  String h = "<!doctype html><html><head><meta charset='utf-8'>"
             "<meta name='viewport' content='width=device-width,initial-scale=1'>"
             "<title>ESP32 LED</title><style>"
             "body{font-family:sans-serif;text-align:center;padding:24px}"
             ".tt{font-size:28px;font-weight:bold;padding:16px;border-radius:12px;margin:16px auto;max-width:320px}"
             ".bat{background:#ffd54f;color:#000}.tat{background:#424242;color:#fff}"
             "a button{font-size:20px;padding:12px 20px;margin:6px;border-radius:10px;border:1px solid #888}"
             "</style></head><body><h1>ESP32 LED</h1>";
  h += on ? "<div class='tt bat'>LED đang BẬT</div>" : "<div class='tt tat'>LED đang TẮT</div>";
  h += "<a href='/on'><button>Bật</button></a>";
  h += "<a href='/off'><button>Tắt</button></a>";
  h += "<a href='/toggle'><button>Toggle</button></a>";
  h += "</body></html>";
  server.send(200, "text/html", h);
}

void veTrangChu() {
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleToggle() { datLed(!on); veTrangChu(); }
void handleOn() { datLed(true); veTrangChu(); }
void handleOff() { datLed(false); veTrangChu(); }

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  datLed(false);
  WiFi.begin(ssid, pass);
  Serial.printf("Dang noi vao %s", ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nIP: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.begin();
  Serial.println("Web server cong 80 da chay");
}

void loop() {
  server.handleClient();
}
