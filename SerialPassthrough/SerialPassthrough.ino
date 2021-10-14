#include "HID-Project.h"

void setup() {
  Serial1.begin(115200);
  Keyboard.begin();
}

void loop() {
  if (Serial1.available()) {  
    Keyboard.print(Serial1.readString());
    delay(50);
    Keyboard.write(KEY_ENTER);
    delay(300);
  }
}
