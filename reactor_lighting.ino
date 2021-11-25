// Author: Krystian Szefler
// Date: 10.05.2021
// CyberLab, NCBJ
// Used board: Adafruit Pro Trinket (16MHz)

// NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
// https://learn.adafruit.com/introducing-pro-trinket/windows-setup

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#include "NeoFade.cpp"
#include "NeoPulse.cpp"
#include "NeoTwinkle.cpp"

// Pins usage
#define LED_TOP 5
#define LED_BOTTOM 6

// Constant
#define NUM_LED_TOP 24
#define NUM_LED_BOTTOM 16

// Templary values for conversion
uint16_t r, g, b;
uint32_t color;

Adafruit_NeoPixel ringTop = Adafruit_NeoPixel(NUM_LED_TOP, LED_TOP, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ringBottom = Adafruit_NeoPixel(NUM_LED_BOTTOM, LED_BOTTOM, NEO_GRB + NEO_KHZ800);

// Color constans
uint32_t whiteColor = ringTop.Color(40, 40, 40);
uint32_t blackColor = ringTop.Color(0, 0, 0);
uint32_t blueColor = ringTop.ColorHSV(65536 * 2 / 3, 255, 255);
uint32_t fadedBlueColor = ringTop.ColorHSV(65536 * 2 / 3, 150, 255);
uint32_t greenColor = ringTop.gamma32(ringTop.Color(0, 150, 0));

//.ColorHSV(hue, saturation, value);
//strip.gamma32(strip.ColorHSV(hue, sat, val));

NeoFade ringTopFade, ringBottomFade;
NeoPulse ringTopPulse;
NeoTwinkle ringBottomTwinkle;

typedef enum {
  standby,
  on,
  error
} States;

States state;

bool inTransition = false;
bool runOnce = false; // it is not necessary to refresh leds if nothing changed,

unsigned long currentTime = millis();
unsigned long startTime = startTime;
unsigned long elapsedTime;
long waitTime = 1234;

void setup() {
  //  Serial.begin(115200);
  //  delay(1000);
  //  Serial.println("\nStart");
  
  ringTop.begin();
  ringTop.setBrightness(255);
  ringTop.show();

  ringBottom.begin();
  ringBottom.setBrightness(255);
  ringBottom.show();

  // blink for startup
  startup();

  state = on;

  ringTopFade.begin();
  ringBottomFade.begin();
  ringTopPulse.begin();
  ringBottomTwinkle.begin(ringBottom);

}

void loop() {
  // state machine
  switch (state) {
    case standby:
      standbyState();
      if (!inTransition) {
        if (to_on()) state = on;
      }
      break;
    case on:
      onState();
      if (!inTransition) {
        if (to_error()) state = error;
      }
      break;
    case error:
      errorState();
      if (!inTransition) {
        if (to_on()) state = on;
      }
      break;
  }
}

//
// ---------------- CONDITIONS FOR STATE TRANSITION ---------------
//
bool to_on() {
  if (time_to_switch()) {
    ringTopFade.setFade(ringTop.getPixelColor(0), whiteColor);
    ringBottomFade.setFade(ringBottom.getPixelColor(0), blueColor);
    runOnce = false;
    return true;
  }
  return false;
}

bool to_error() {
  if (time_to_switch()) {
    ringTopFade.setFade(ringTop.getPixelColor(0), blackColor);
    ringBottomFade.setFade(ringBottom.getPixelColor(0), blackColor);
    ringTopPulse.reset();
    runOnce = false;
    return true;
  }
  return false;
}

//
// --------------- STATES ---------------
//
void standbyState() {
  if (transiition() && !runOnce) {
    // Top LEDs
    ringTop.fill(greenColor);
    ringTop.show();
    
    // Bottom LEDs
    ringBottom.fill(greenColor);
    ringBottom.show();
    
    runOnce = true;
  }
}

void onState() {
  if (transiition()) {

    // Top LEDs white lighting
    if (!runOnce) {
      ringTop.fill(whiteColor);
      ringTop.show();
      
      runOnce = true;
    }

    // Bottom LEDs sparkling blue light
    ringBottomTwinkle.getNext();
    ringBottom.show();
  }
}

void errorState() {
  if (transiition()) {

    // Top and bottom LEDs pulsating red lighting
    ringTopPulse.getNext(color);
    ringTop.fill(color);
    ringTop.show();

    ringBottom.fill(color);
    ringBottom.show();
  }
}

bool transiition() {
  if (!ringTopFade.isEnded()) {
    if (ringTopFade.getNext(r, g, b)) {

      ringTop.fill(ringTop.Color(r, g, b));
      ringTop.show();
      // Serial.print((String)" (" + r + "," + g + "," + b + ")");
    }

    if (ringBottomFade.getNext(r, g, b)) {
      ringBottom.fill(ringBottom.Color(r, g, b));
      ringBottom.show();
      //  Serial.print((String)" (" + r + "," + g + "," + b + ")");
    }

    inTransition = true;
    return false;
  }
  inTransition = false;
  return true; // finished transiition
}

//
// ---------------- OTHER ---------------
//
inline void startup() {
  delay(250);
  ringTop.fill(greenColor);
  ringBottom.fill(greenColor);
  ringTop.show();
  ringBottom.show();
  delay(250);
  ringTop.fill(blackColor);
  ringBottom.fill(blackColor);
  ringTop.show();
  ringBottom.show();
  delay(250);
  ringTop.fill(greenColor);
  ringBottom.fill(greenColor);
  ringTop.show();
  ringBottom.show();
  delay(250);
  ringTop.fill(blackColor);
  ringBottom.fill(blackColor);
  ringTop.show();
  ringBottom.show();
  delay(250);
}

bool time_to_switch() {

  currentTime = millis();
  elapsedTime = currentTime - startTime;

 if(elapsedTime > waitTime)
      waitTime = random(10, 20) * 1234;
      startTime = currentTime;
      return true;
    }
  return false;
}

//inline void print_color (uint32_t color) {
//  Serial.print((String)" (" + (String)((color >> 16) & 0xFF) + "," + (String)((color >> 8) & 0xFF) + "," + (String)(color & 0xFF) + ")");
//}
//
//inline void print_blue_color (uint32_t color) {
//  byte b = (color & 0xFF);
//  Serial.print((String)" (" + (b < 10 ? "  " : (b < 100 ? " " : "")) + (String)b + ")");
//}
