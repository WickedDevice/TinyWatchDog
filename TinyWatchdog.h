#ifndef _TINYWATCHDOG_LIB_H
#define _TINYWATCHDOG_LIB_H

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <stdint.h>

class TinyWatchdog {
  private:
    uint8_t pin;
    uint16_t checkcrc(uint8_t * buffer);

    // SoftwareSerial stuff
    void tx_pin_write(uint8_t pin_state);
    static inline void tunedDelay(uint16_t delay);
    size_t write(uint8_t b);    
    uint8_t _transmitBitMask;
    volatile uint8_t *_transmitPortRegister;
    uint16_t _tx_delay;        
  public:
    TinyWatchdog(uint8_t pin = 14);
    void begin(uint16_t min_window_ms, uint16_t max_window_ms);
    void pet(void);
	void force_reset(void);
};


#endif //_TINYWATCHDOG_LIB_H

