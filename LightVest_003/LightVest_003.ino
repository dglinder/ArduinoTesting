#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINT(x)     Serial.print (x)
 #define DEBUG_PRINTDEC(x)  Serial.print (x, DEC)
 #define DEBUG_PRINTLN(x)   Serial.println (x)
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTDEC(x)
 #define DEBUG_PRINTLN(x)
#endif 

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

unsigned long time;
uint32_t foo;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  Serial.begin(9600);      // open the serial port at 9600 bps:    

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

//  colorSet(strip.Color(255, 0, 0)); // Red
//  strip.show();
//  delay(1000);
//  colorSet(strip.Color(0, 0, 0)); // Black
//  strip.show();
}

void loop() {
  time = millis();

  showLapel( (time / 1000) % 4 );
//  showLapel(0);
  showShouler(1);
  showBack(  (time / 333) % 2 );

  strip.show();
}

// Vest layout
#define cLapelStart_A     0
#define cLapelStart_B    60
#define cLapelEnd_A       5
#define cLapelEnd_B      54

#define cShoulderStart_A  6
#define cShoulderStart_B 53
#define cShoulderEnd_A   12
#define cShoulderEnd_B   47

#define cBackStart_A     13
#define cBackStart_B     46
#define cBackEnd_A       29
#define cBackEnd_B       30

void showLapel(uint8_t pattern) {
  // Display the specific pattern requested.
  switch (pattern) {
    case 0:
        showLapel_1(strip.Color(64, 64, 64));
      break;
    case 1:
        showLapel_1(strip.Color(128, 0, 0));
      break;
    case 2:
        showLapel_1(strip.Color(0, 128, 0));
      break;
    case 3:
        showLapel_1(strip.Color(0, 0, 128));
      break;
    default:
      break;
  }
}

void showShouler(uint8_t pattern) {
  // Display the specific pattern requested.
  switch (pattern) {
    case 1:
        showShoulder_1(strip.Color(255, 0, 0));
      break;
    default:
      break;
  }
}

void showBack(uint8_t pattern) {
  // Display the specific pattern requested.
  switch (pattern) {
    case 0:
        showBack_1(strip.Color(0, 0, 64));
      break;
    case 1:
        showBack_1(strip.Color(255, 0, 0));
      break;
    default:
      break;
  }
}

void showBack_1(uint32_t c) {
  // Rainbow along the shoulder
  uint32_t i;
  //uint32_t j = millis()/2;
  
  for(i=cBackStart_A; i<=cBackEnd_A; i++) {
    strip.setPixelColor(i, c);
    strip.setPixelColor((cBackEnd_B-(cBackStart_A-i)), c);
  }
}

void showLapel_1 (uint32_t c) {
  // Blink the lights color "c" every second
  uint32_t dcolor = millis() / 500;
  //dcolor = dcolor % 2;
  //dcolor = dcolor * c;
  dcolor = c;
  
  DEBUG_PRINT("LAPEL: time / color : ");
  DEBUG_PRINT(millis());
  DEBUG_PRINT(" / ");
  DEBUG_PRINT(dcolor);
  DEBUG_PRINTLN();
  for(uint16_t i=0; i<=(cLapelEnd_A-cLapelStart_A); i++) {
    strip.setPixelColor(cLapelStart_A + i, dcolor);
    strip.setPixelColor(cLapelEnd_B + i, dcolor);
  }
}

void showShoulder_1(uint32_t c) {
  // Rainbow along the shoulder
  uint32_t i;
  uint32_t j = millis()/2;
  for(i=cShoulderStart_A; i<=cShoulderEnd_A; i++) {
    //c = Wheel((i+j) & 255);
    c = Wheel((j) & 255);
    strip.setPixelColor(i, c);
    strip.setPixelColor((cShoulderEnd_B-(cShoulderStart_A-i)), c);
  }
}

void colorSet(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}