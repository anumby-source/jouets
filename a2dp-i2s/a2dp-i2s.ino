#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

void setup() {
    auto cfg = i2s.defaultConfig();
    cfg.pin_bck = 4;// 14
    cfg.pin_ws = 15;
    cfg.pin_data = 2;//22
    i2s.begin(cfg);

    a2dp_sink.start("MyMusic");
}

void loop() {
}
