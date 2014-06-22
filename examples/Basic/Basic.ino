#include <TinyWatchdog.h>

TinyWatchdog tinyWDT;

void setup(){
  tinyWDT.begin(1000, 10000); // window 1-10s
}

void loop(){
  tinyWDT.pet();
  delay(5000); // leave enough time to not trip the low window
}
