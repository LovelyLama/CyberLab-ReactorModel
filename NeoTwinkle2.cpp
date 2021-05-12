#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

#define NUM_LED 16

// https://gist.github.com/suhajdab/9716028

class NeoTwinkle
{
  public:
    void begin(Adafruit_NeoPixel &neopixel)
    {
      strip = neopixel;
      _timeInterval = 700;
      reset();
    }

    inline void reset(void)
    {
      for (uint8_t l = 0; l < NUM_LED; l++) {
        blueStates[l] = 255;
      }
    }

    bool getNext()
    {
      if (millis() - _timeStart < _timeInterval)
        return false;
      _timeStart = millis();

      // from time to time change state of one light
      //      if (random(2) == 1) {
      uint8_t i = random(NUM_LED);
      //      if (blueStates[i] < 1) {
      blueStates[i] = random(255);
      //      }
      //      }

      for (uint8_t led = 0; led < NUM_LED; led++) {
        // if given led is lightened
        if (blueStates[led] > 1) {
          // push data to strip
          strip.setPixelColor(led, 0, 0, blueStates[led]);

          // fading a led
          blueStates[led] = blueStates[led] > 1 ? blueStates[led] * fadeRate : 0;

        } else {
          strip.setPixelColor(led, 0, 0, 0);
        }
      }
      return true;
    }

  private:
    byte blueStates[NUM_LED];
    float fadeRate = 0.96;
    Adafruit_NeoPixel strip;

    // timing parameters
    uint32_t _timeStart; // animation time start
    uint32_t _timeInterval; // interval between color changes
};
