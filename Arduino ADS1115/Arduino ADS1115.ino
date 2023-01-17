#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

void setup(void) {
  Serial.begin(9600);
  ads.begin();
  ads.startADCReading(ADS1X15_REG_CONFIG_MUX_SINGLE_0, true);
}

void loop(void) {
  int16_t results = ads.getLastConversionResults();
  Serial.println(results);
  delay(100);
}