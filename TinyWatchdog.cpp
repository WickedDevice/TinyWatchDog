#include "TinyWatchdog.h"
#include <stdint.h>
#include <util/crc16.h>

#define INITIALIZATION_MESSAGE_LENGTH 6

TinyWatchdog::TinyWatchdog(uint8_t pin){
  this->pin = pin;
}

// set the pin to a SoftSerial transmit pin
void TinyWatchdog::begin(uint16_t min_window_ms, uint16_t max_window_ms){
  uint8_t buffer[6] = {0};
  uint16_t checksum = 0;
  uint8_t ii = 0;
  
  //initialize SoftwareSerial transmitter
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);  
  delay(10);
  _transmitBitMask = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  _transmitPortRegister = portOutputRegister(port);  
  _tx_delay = 471; // 4800 baud value from SoftwareSerial library
  
  buffer[0] = min_window_ms & 0xff;
  min_window_ms >>= 8;
  buffer[1] = min_window_ms & 0xff;  
  
  buffer[2] = max_window_ms & 0xff;  
  max_window_ms >>= 8;
  buffer[3] = max_window_ms & 0xff;    
  
  checksum = checkcrc(buffer);
  buffer[4] = checksum & 0xff;
  checksum >>= 8;
  buffer[5] = checksum & 0xff;
  
  /* test pattern
  buffer[0] = 0x01; 
  buffer[1] = 0xA0; 
  buffer[2] = 0x10; 
  buffer[3] = 0x33; 
  buffer[4] = 0xB7; 
  buffer[5] = 0x99;
  */
  
  write(0x5A); // first header byte
  write(0x96); // second header byte
  
  for(ii = 0; ii < INITIALIZATION_MESSAGE_LENGTH; ii++){
    write(buffer[ii]);
  }  
  
  // digitalWrite(pin, HIGH); // happens automatically at the end of each write
}

void TinyWatchdog::pet(void){

  digitalWrite(pin, LOW);
  
  // Tiny Watchdog samples once per millisecond, 
  // and it gives the host 10ms to release before the new window assessment starts
  // to prevent an immediate "too early petting" condition
  // a 3ms negative pulse satisfies this condition   
  
  delay(3);
    
  digitalWrite(pin, HIGH);  
}

void TinyWatchdog::force_reset(void){

  digitalWrite(pin, LOW);

  delay(500);
}

uint16_t TinyWatchdog::checkcrc(uint8_t * buffer){
  uint8_t crc = 0, ii = 0;

  for (ii = 0; ii < INITIALIZATION_MESSAGE_LENGTH - 2; ii++){
    crc = _crc16_update(crc, buffer[ii]);
  }

  return crc; 
}

void TinyWatchdog::tx_pin_write(uint8_t pin_state){
  if (pin_state == LOW)
    *_transmitPortRegister &= ~_transmitBitMask;
  else
    *_transmitPortRegister |= _transmitBitMask;
}

const int XMIT_START_ADJUSTMENT = 5; // from SoftwareSerial library
size_t TinyWatchdog::write(uint8_t b){
  uint8_t oldSREG = SREG;
  cli();  // turn off interrupts for a clean txmit

  // Write the start bit
  tx_pin_write(LOW);
  tunedDelay(_tx_delay + XMIT_START_ADJUSTMENT);

  // Write each of the 8 bits
  for (byte mask = 0x01; mask; mask <<= 1)
  {
    if (b & mask) // choose bit
      tx_pin_write(HIGH); // send 1
    else
      tx_pin_write(LOW); // send 0
  
    tunedDelay(_tx_delay);
  }

  tx_pin_write(HIGH); // restore pin to natural state


  SREG = oldSREG; // turn interrupts back on
  tunedDelay(_tx_delay);
  
  return 1;
}

inline void TinyWatchdog::tunedDelay(uint16_t delay) { 
  uint8_t tmp=0;

  asm volatile("sbiw    %0, 0x01 \n\t"
    "ldi %1, 0xFF \n\t"
    "cpi %A0, 0xFF \n\t"
    "cpc %B0, %1 \n\t"
    "brne .-10 \n\t"
    : "+r" (delay), "+a" (tmp)
    : "0" (delay)
    );
}
