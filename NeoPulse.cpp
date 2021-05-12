#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

// https://forum.arduino.cc/t/logarithmic-scaling-for-led-dimming/144333/11

#define HOLD_C 20 // interval wait time before changing direction of pulse

class NeoPulse
{
  public:
    void begin(void)
    {
      _period = 1000;
      a = 0.33;
      Pmax = pow(255, a);
      maxBrightness = 255;
      minBrightness = 140;
      reset();
    }

    inline void reset(void)
    {
      dir = 1;
      bright = 0;
      hold = 0;
      calcTimeInterval();
    }

    inline void setLimits(byte max, byte min) {
      if (max > min) {
        maxBrightness = max;
        minBrightness = min;
      }
    }

    inline void setPeriod(uint16_t p) {
      _period = p;
      calcTimeInterval();
    }

    bool getNext(uint32_t &color)
    {
      if (millis() - _timeStart < _timeInterval)
        return false;

      _timeStart = millis();

      if (hold) {
        --hold;
      } else {

        if (bright <= minBrightness) {
          ++bright;
          dir = 1;
        } else if (bright >= maxBrightness) {
          --bright;
          dir = -1;
        } else {
          bright += dir;
        }

        color = neopixel.gamma32(neopixel.ColorHSV(0, 255,
                                 int(pow(Pmax * ((float)bright / 255.0), 1 / a) + 0.5) // logarithmic scaling
                                                  ));

        if ((bright == minBrightness && dir == -1) || (bright == maxBrightness  && dir == 1)) {
          hold = HOLD_C;
          dir *= -1; //set dir to +1 or -1
        }
      }

      return true;
    }

  private:
    byte dir; //direction
    byte bright; //brightness
    byte maxBrightness;
    byte minBrightness;
    byte hold;
    Adafruit_NeoPixel neopixel;

    // constans
    float a;
    float Pmax;

    // timing parameters
    uint16_t _period;    // fade duration in ms
    uint16_t _steps;    // fade duration in ms
    uint32_t _timeStart; // animation time start
    uint32_t _timeInterval; // interval between color changes

    void calcTimeInterval(void)
    {
      _timeInterval = _period / (maxBrightness - minBrightness);
      if (_timeInterval == 0) _timeInterval = 1;
    }

};
