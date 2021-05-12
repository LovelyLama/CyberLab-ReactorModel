#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

#define NUM_LED 16
#define HOLD_C 2 // interval wait time before starting twinkle

// https://gist.github.com/suhajdab/9716028

class NeoTwinkle
{
  public:
    void begin(Adafruit_NeoPixel &neopixel)
    {
      strip = neopixel;
      _timeInterval = 500;
      reset();
    }

    inline void reset(void)
    {
      for (byte i = 0; i < NUM_LED; i += NUM_LED / 4)
        strip.setPixelColor(i, whiteColor);
      hold = HOLD_C;
    }

    bool getNext()
    {
      if (millis() - _timeStart < _timeInterval)
        return false;
      _timeStart = millis();

      if (hold) {
        --hold;
      } else {
        for (byte i = 0; i < changeAtTime; ++i) {
          uint8_t r = random(NUM_LED);
          blueStates[r] ^= 1;
          strip.setPixelColor(r, blueStates[i] == 1 ? whiteColor : blueColor);
        }
      }

      return true;
    }

  private:
    byte blueStates[NUM_LED];
    byte hold;
    byte changeAtTime = 3; // change 3 diodes at at time
    Adafruit_NeoPixel strip;
    uint32_t blueColor = strip.ColorHSV(65536 * 2 / 3, 255, 255);
    uint32_t whiteColor = strip.Color(255, 255, 255);

    // timing parameters
    uint32_t _timeStart; // animation time start
    uint32_t _timeInterval; // interval between color changes
};
