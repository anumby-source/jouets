// arnaudrco mettre à jour l'adresse MAC broadcastAddress

#include <espnow.h>//https://github.com/esp8266/Arduino/blob/master/tools/sdk/include/espnow.h
#include <ESP8266WiFi.h>

// A8:48:FA:DD:27:6D


uint8_t broadcastAddress[] = {0xA8, 0x48, 0xFA, 0xDD , 0x27, 0x6D};// REPLACE WITH RECEIVER MAC ADDRESS
// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
 char a[32];
 int b;
 float c;
 String d;
 bool e;
} struct_message;
struct_message myData;

unsigned long previousTime=0;

// callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t status) {
 Serial.print(F("\r\n Master packet sent:\t"));
 Serial.println(status == 0 ? "Delivery Success" : "Delivery Fail");
}
void setup() {
 // Init Serial Monitor
 Serial.begin(115200);
 // Set device as a Wi-Fi Station
 WiFi.mode(WIFI_STA);
 // Init ESP-NOW
 if (esp_now_init() != 0) {
     Serial.println(F("Error initializing ESP-NOW"));
    return;
 }
 Serial.print(F("\nReciever initialized : "));
 Serial.println(WiFi.macAddress());
 
 // Define Send function
 esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
 esp_now_register_send_cb(OnDataSent);
 // Register peer
 esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
void loop() {
  if((millis() -previousTime)>1000){
        // Set values to send
        strcpy(myData.a, "data type char");
        myData.b = random(1, 20);
        myData.c = 1.2;
        myData.d = "anumby";
        myData.e = false;
        // Send message via ESP-NOW
        uint8_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
        if (result == 0) {
            Serial.println(F("Sent with success"));
        }
        else {
            Serial.println(F("Error sending the data"));
        }
        previousTime=millis();
  }
}
