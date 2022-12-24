const int HX_OUT_PIN = 2; // Connect HX710B 'OUT' pin to Arduino pin 2
const int HX_SCK_PIN = 3; // Connect HX710B 'SCK' pin to Arduino pin 3

enum HX_MODE { NONE, DIFF_10Hz, TEMP_40Hz, DIFF_40Hz};
const byte HX_MODE = DIFF_40Hz; // Change mode here

void setup() {
  pinMode(HX_SCK_PIN, OUTPUT);
  pinMode(HX_OUT_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.println(ReadCount());
}

unsigned long ReadCount() {

  // pulse clock to set the mode for next reading
  for (char i = 0; i < HX_MODE; i++) {
    digitalWrite(HX_SCK_PIN, HIGH);
    digitalWrite(HX_SCK_PIN, LOW);
  }

  // wait for reading to finish
  while (digitalRead(HX_OUT_PIN)) {}

  // read the 24-bit value
  byte data[3];
  for (byte j = 3; j--;) {
    data[j] = shiftIn(HX_OUT_PIN, HX_SCK_PIN, MSBFIRST);
  }
  data[2] ^= 0x80;  // flip the out of range error bit

  // convert the 3 bytes to a long integer
  long result;
  result += (long)data[2] << 16;
  result += (long)data[1] << 8;
  result += (long)data[0];

  return result;
}