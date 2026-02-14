#include <driver/i2s.h>

// I2S configuration
#define I2S_NUM         I2S_NUM_0  // Use I2S port 0

/*
#define I2S_BCK_IO      14         // Serial clock (SCK)
#define I2S_WS_IO       15         // Word select (WS)
#define I2S_DATA_IN_IO  32         // Serial data (SD)
*/

#define I2S_BCK_IO      0         // Serial clock (SCK)
#define I2S_WS_IO       1         // Word select (WS)
#define I2S_DATA_IN_IO  2         // Serial data (SD)

// I2S configuration structure
void setupI2S() {
  i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // Master receive mode
    .sample_rate = 16000,                              // Sampling rate
    .bits_per_sample = I2S_BITS_PER_SAMPLE_24BIT,      // 24-bit audio
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,       // Single channel (left)
    .communication_format = I2S_COMM_FORMAT_I2S,       // I2S format
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,          // Interrupt level
    .dma_buf_count = 8,                                // Number of DMA buffers
    .dma_buf_len = 64                                  // Buffer length
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCK_IO,  // Serial clock pin
    .ws_io_num = I2S_WS_IO,    // Word select pin
    .data_out_num = -1,        // Not used (output pin)
    .data_in_num = I2S_DATA_IN_IO // Serial data input pin
  };

  // Install and start I2S driver
  i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM, &pin_config);
}

void setup() {
  Serial.begin(115200);
    Serial.println("test");
  setupI2S();
  Serial.println("INMP441 Microphone Initialized");
}

void loop() {
  uint8_t data[128];  // Buffer to store audio data
  size_t bytesRead;

  // Read audio data from I2S
  i2s_read(I2S_NUM, data, sizeof(data), &bytesRead, portMAX_DELAY);

  // Process or transmit the audio data as needed
  //Serial.print("Bytes read: ");
  Serial.println(bytesRead);
}
