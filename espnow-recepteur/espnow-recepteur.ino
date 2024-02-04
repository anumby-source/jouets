#include <espnow.h>// https://github.com/esp8266/Arduino/blob/master/tools/sdk/include/espnow.h
#include <ESP8266WiFi.h>

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
 char a[32];
 int b;
 float c;
 String d;
 bool e;
} struct_message;
struct_message myData;
// callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
 memcpy(&myData, incomingData, sizeof(myData));
 Serial.print("Bytes received: ");
 Serial.println(len);
 Serial.print("Char: ");
 Serial.println(myData.a);
 Serial.print("Int: ");
 Serial.println(myData.b);
 Serial.print("Float: ");
 Serial.println(myData.c);
 Serial.print("String: ");
 Serial.println(myData.d);
 Serial.print("Bool: ");
 Serial.println(myData.e);
 Serial.println();
}
void setup() {
 // Initialize Serial Monitor
 Serial.begin(115200);
 // Set device as a Wi-Fi Station
 WiFi.mode(WIFI_STA);
 // Init ESP-NOW
 if (esp_now_init() != 0) {
     Serial.println("Error initializing ESP-NOW");
    return;
 }
 Serial.print(F("\nReciever initialized : "));
 Serial.println(WiFi.macAddress());
 // Define receive function
 esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
 esp_now_register_recv_cb(OnDataRecv);
}
void loop() {
}
