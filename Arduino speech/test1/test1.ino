#include "speech.h"

void setup() {
}

void loop() {
  for (;;) {
    for (int i = 0; i < 10; i++) {
      play_digit(i);
    }
    play_point();
  }
}