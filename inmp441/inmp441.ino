#include <ESP_I2S.h>

I2SClass I2S;

/*

constexpr int PIN_BCLK = 26;  // SCK de l'INMP441
constexpr int PIN_WS   = 25;  // WS
constexpr int PIN_SD   = 33;  // SD, données du micro
*/

#define PIN_BCLK      0         // Serial clock (SCK)
#define PIN_WS       1         // Word select (WS)
#define PIN_SD  2         // Serial data (SD)

#define ledPin 8

void setup() {
  Serial.begin(115200);
  // test fading de la LED 
  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
  // INMP441 : I2S, réception seulement, 24 kHz, échantillons 32 bits
  I2S.setPins(PIN_BCLK, PIN_WS, -1, PIN_SD, -1);

  if (!I2S.begin(I2S_MODE_STD,
                 24000,
                 I2S_DATA_BIT_WIDTH_32BIT,
                 I2S_SLOT_MODE_MONO)) {
    Serial.println("Erreur : initialisation I2S impossible");
    while (true) delay(1000);
  }

  Serial.println("INMP441 prêt");
}

void loop() {
  int32_t echantillon;
  int64_t somme = 0;
  const int nombreEchantillons = 256;

  for (int i = 0; i < nombreEchantillons; i++) {
    if (I2S.readBytes((char *)&echantillon, sizeof(echantillon))
        != sizeof(echantillon)) {
      Serial.println("Lecture I2S impossible");
      return;
    }

    // Le signal utile du INMP441 est sur les bits de poids fort. 24 bits 
    int32_t audio = echantillon >> 8;
    somme += (int64_t)audio * audio;
  }

  float rms = sqrt((double)somme / nombreEchantillons);
  Serial.println(rms);
  rms= rms / 4096 ; // 2 puissance 12
  if (rms > 255 ) rms=255;
    analogWrite(ledPin,255 - rms);
}
