// arnaudrco affichage sur un écran OLED
// SDA D3 
// SCL D4


#include <espnow.h>// https://github.com/esp8266/Arduino/blob/master/tools/sdk/include/espnow.h
#include <ESP8266WiFi.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SDA D3
#define SCL D4

// Adafruit_SSD1306 display = Adafruit_SSD1306();
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

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
    pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);  
 // Initialize Serial Monitor
 Serial.begin(115200);
 Wire.begin(SDA,SCL); // SDA SCL
    
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  display.clearDisplay();
  // text display big!
  display.setTextSize(3);
  display.setTextColor(WHITE);
        display.print("anumby");
  display.display();

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
  digitalWrite(LED_BUILTIN,  myData.e);
        display.clearDisplay();
      display.setCursor(0,0);
                    display.println( myData.d);
              display.print((int) myData.b);
              display.display();

}
