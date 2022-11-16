# Arduino Notes

## Scan I2C Devices

```c
#include <BMP280_DEV.h>
#include <Wire.h>

BMP280_DEV bmp280;

void setup() {
  Serial.begin(115200);
  Serial.println("Searching for I2C devices...");
  Wire.begin();
  scanI2C();
}

void loop() {
}

void scanI2C() {
  byte error, address;
  int nDevices = 0;
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("scan complete\n");
}
```

```
20:11:38.154 -> Scanning
20:11:38.193 -> I2C device found at address 0x76  !
20:11:38.193 -> scan complete
```