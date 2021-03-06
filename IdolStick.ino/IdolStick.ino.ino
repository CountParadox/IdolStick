/* button_demoReel100
 *
 * Originally By: Mark Kriegsman
 * 
 * Modified for EEPROM and button usage by: Andrew Tuline
 * 
 * Modified for use in Weeb Penlight by Lewys Martin
 *
 * Date: January, 2017
 *
 * This takes Mark's DemoReel100 routine and adds the ability to change modes via button press.
 * It also allows the user to save the current mode into EEPROM and use that as the starting mode.
 *
 * Instructions:
 * 
 * Program reads display mode from EEPROM and displays it.
 * Click to change to the next mode.
 * Hold button for > 1 second to write the current mode to EEPROM.
 * Double click to reset to mode 0.
 * 
 * There's also debug output to the serial monitor . . just to be sure.
 * 
 * LEDs are connected to PIN 10
 * Button is on PIN 6
 * There are 14 LEDs, in two parrallel groups of 7, so make sure NUM_LEDS is set to 7
 * The LED_TYPE is WS2812 and the COLOR_ORDER is GRB
 * 
 * Requirements:
 * 
 * In addition, to FastLED, you need to download and install JChristensen's button library, available at:
 * 
 * https://github.com/JChristensen/Button
 * 
 */

#define buttonPin 6                                           // input pin to use as a digital input
#include "jsbutton.h"                                         // Nice button routine by Jeff Saltzman

#include "EEPROM.h"
#include "FastLED.h"

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif


// Fixed definitions cannot change on the fly.
#define LED_DT 10                                             // Data pin to connect to the strip.
#define LED_CK 16                                             // Clock pin for WS2801 or APA102.
#define COLOR_ORDER GRB                                       // It's GRB for WS2812 and BGR for APA102.
#define LED_TYPE WS2812                                       // Using APA102, WS2812, WS2801. Don't forget to modify LEDS.addLeds to suit.
#define NUM_LEDS 7                                            // Number of LED's.




// Definition for the array of routines to display.
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//Mode and EEPROM variables
uint8_t maxMode = 13;                                          // Maximum number of display modes. Would prefer to get this another way, but whatever.
int eepaddress = 0;

// Global variables can be changed on the fly.
uint8_t max_bright = 200;                                     // Overall brightness definition. It can be changed on the fly.

struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.

uint8_t gCurrentPatternNumber = 0;                            // Index number of which pattern is current
uint8_t gHue = 0;                                             // rotating "base color" used by many of the patterns

typedef void (*SimplePatternList[])();                        // List of patterns to cycle through.  Each is defined as a separate function below.



void setup() {

  Serial.begin(57600);                                        // Initialize serial port for debugging.
  delay(1000);                                                // Soft startup to ease the flow of electrons.

  pinMode(buttonPin, INPUT);                                  // Set button input pin
  digitalWrite(buttonPin, HIGH );
  
//  LEDS.addLeds<LED_TYPE, LED_DT, LED_CK, COLOR_ORDER>(leds, NUM_LEDS);  // Use this for WS2801 or APA102
  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);  // Use this for WS2812

  FastLED.setBrightness(max_bright);
  set_max_power_in_volts_and_milliamps(5, 500);               // FastLED Power management set at 5V, 500mA.

  gCurrentPatternNumber = EEPROM.read(eepaddress);

  if (gCurrentPatternNumber > maxMode) gCurrentPatternNumber = 0;   // A safety in case the EEPROM has an illegal value.

  
  Serial.print("Starting ledMode: ");
  Serial.println(gCurrentPatternNumber);

} // setup()



SimplePatternList gPatterns = {white, green, red, blue, aqua, miku, yellow, orange, pink, purple, rainbow, effect1, effect2 };               // Don't know why this has to be here. . . (me neither - LM)

 

void loop() {

  readbutton();
  
  EVERY_N_MILLISECONDS(50) {
    gPatterns[gCurrentPatternNumber]();                       // Call the current pattern function once, updating the 'leds' array    
  }

  EVERY_N_MILLISECONDS(20) {                                  // slowly cycle the "base color" through the rainbow
    gHue++;
  }

  FastLED.show();

} // loop()



void readbutton() {                                           // Read the button and increase the mode

  uint8_t b = checkButton();

  if (b == 1) {                                               // Just a click event to advance to next pattern
    gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
    Serial.println(gCurrentPatternNumber);
  }

  if (b == 2) {                                               // A double-click event to reset to 0 pattern
    gCurrentPatternNumber = 0;
    Serial.println(gCurrentPatternNumber);
  }

  if (b == 3) {                                               // A hold event to write current pattern to EEPROM
    EEPROM.write(eepaddress,gCurrentPatternNumber);
    Serial.print("Writing: ");
    Serial.println(gCurrentPatternNumber);    
  }

} // readbutton()




//--------------------[ Effects are below here ]------------------------------------------------------------------------------



void white() {

fill_solid(leds, NUM_LEDS, CRGB::White);  

  
} // White()


void green() {

fill_solid(leds, NUM_LEDS, CRGB::Green);  

  
} // Green()



void red() {

  fill_solid(leds, NUM_LEDS, CRGB::Red);  


} // Red()



void blue() {                                             // Random colored speckles that blink in and fade smoothly.

fill_solid(leds, NUM_LEDS, CRGB::Blue);  

  
} // Blue()



void aqua() {                                              // A colored dot sweeping back and forth, with fading trails.

fill_solid(leds, NUM_LEDS, CRGB::Aqua);  

  
} // Aqua()

void miku() {                                              // A colored dot sweeping back and forth, with fading trails.

fill_solid(leds, NUM_LEDS, CRGB(50,255,50));  

  
} // Miku Green()

void yellow() {                                                  // Colored stripes pulsing at a defined Beats-Per-Minute.

 fill_solid(leds, NUM_LEDS, CRGB::Yellow);  
  
} // Yellow()



void orange() {                                               // Eight colored dots, weaving in and out of sync with each other.

fill_solid(leds, NUM_LEDS, CRGB::Orange);  

  
  
} // Orange()

void pink() {                                               // Eight colored dots, weaving in and out of sync with each other.

fill_solid(leds, NUM_LEDS, CRGB::DeepPink);  

  
  
} // Pink()

void purple() {                                               // Eight colored dots, weaving in and out of sync with each other.

fill_solid(leds, NUM_LEDS, CRGB::Purple);  

  
  
} // Purple()


void rainbow() {

  fill_rainbow(leds, NUM_LEDS, gHue, 7);                      // FastLED's built-in rainbow generator.
  
} // rainbow()

void effect1() {                                               // Eight colored dots, weaving in and out of sync with each other.

 // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
  
  
} // Sweeping effect()

void effect2() {  
  
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
  
  
} // Confetti and shit()


