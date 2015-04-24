#include <TinyWatchdog.h>

TinyWatchdog tinyWDT;
#define TEST1

#ifdef TEST1 
// normal configuration
void setup(){
  Serial.begin(115200);
  tinyWDT.begin(100, 10000); // window 1-10s
  Serial.println("Hello");
}

void loop(){
  Serial.println("Pet.");
  tinyWDT.pet();
  delay(500); // leave enough time to not trip the low window
}
#endif

#ifdef TEST2 
// normal configuration and petting at the lower edge of the window
void setup(){
  Serial.begin(115200);
  Serial.println("Hello.");
  tinyWDT.begin(500, 10000); // window 1-10s
}

void loop(){
  tinyWDT.pet();
  delay(550); // leave enough time to not trip the low window
}
#endif

#ifdef TEST3
// normal configuration and petting at the upper edge of the window
void setup(){
  Serial.begin(115200);
  Serial.println("Hello.");
  tinyWDT.begin(500, 1000); // window 1-10s
}

void loop(){
  tinyWDT.pet();
  delay(900); // leave enough time to not trip the low window
              // 996 because the pet takes 3ms
}
#endif

#ifdef TEST4
// normal configuration and petting before the lower edge of the window
void setup(){
  Serial.begin(115200);
  Serial.println("Hello.");
  tinyWDT.begin(500, 1000); // window 1-10s
}

void loop(){
  tinyWDT.pet();
  delay(449); // leave enough time to not trip the low window
}
#endif

#ifdef TEST5
// normal configuration and petting after the upper edge of the window
void setup(){
  Serial.begin(115200);
  Serial.println("Hello.");
  tinyWDT.begin(500, 1000); // window 1-10s
}

void loop(){
  tinyWDT.pet();
  delay(1101); // leave enough time to not trip the low window
}
#endif

#ifdef TEST6
// incorrect configuration, upper is lower than lower
void setup(){
  Serial.begin(115200);
  Serial.println("Hello.");
  tinyWDT.begin(1000, 500); // window 1-10s
}

void loop(){
  tinyWDT.pet();
  delay(750); // leave enough time to not trip the low window
}
#endif
