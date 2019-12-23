#include "FastLED.h"

#define NUM_LEDS 300        // Velicina koriscene trake
#define updateLEDS 1        // Koliko dioda se updejtuje po sekundi
#define COLOR_SHIFT 1000  // Vreme izmedju promene boje (u ms)
CRGB leds[NUM_LEDS];        // niz led dioda koji obradjujemo

#define RefreshRate 0       // Koliko cesto se cita zvuk (u ms)
// Input/Output
#define DATA_PIN 6          // Izlaz koji kontrolise diode
#define BRIGHT_PIN 0         // audio senzor(mikrofon)

// Unutrasnje promenljive
unsigned long setTime = COLOR_SHIFT;
int shiftRed = 0;
int shiftGreen = 0;
int shiftBlue = 0;
long randomNum;

// Struktura rgb
struct color {
  int r;
  int g;
  int b;
};
typedef struct color Color;


void setup() { 
    Serial.begin(9600);
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    pinMode(A0, INPUT);

    for(int i = 0; i < NUM_LEDS ; i++) {  //Ugasi sve diode
      leds[i] = CRGB(0,0,0);
    }
    randomSeed(analogRead(3));
    FastLED.show();
}

void loop() { 
  unsigned long time = millis();
  Serial.println(time);
  long timeRed = time * random(10);
  long timeBlue = time * random(10);
  
  shiftRed = time % 256;
  shiftGreen = timeRed % 256;
  shiftBlue = timeBlue % 256;

/*
  // Promeni boje za po 200 posle odredjenog vremena (setTime)
  if(time / (double)setTime >= 1) {
    setTime = time + COLOR_SHIFT;
    randomNum = random(66);
    shiftRed = shiftRed + randomNum;
    randomNum = random(33);
    shiftGreen = shiftGreen + randomNum;
    randomNum = random(33);
    shiftBlue = shiftBlue + randomNum;
    
    if(shiftRed >= 254) {
      shiftRed = 0;
    }
    if(shiftGreen >= 254) {
      shiftRed = 0;
    }
    if(shiftBlue >= 254) {
      shiftRed = 0;
    }
  }
*/
  // Shift all LEDs to the right by updateLEDS number each time
  for(int i = 149; i>=0; i--) {
    leds[i+1]=leds[i];
  }
  for (int i = 151; i<=NUM_LEDS - 1; i++) {
    leds[i-1]=leds[i];
  }

  // Get the pitch and brightness to compute the new color
  Color nc;
  int s;
  s = analogRead(BRIGHT_PIN);
  double bright = convBrightness(s);
  bright = bright * 0.2;
  
  nc.r = bright * shiftRed;
  nc.g = bright * shiftGreen;
  nc.b = bright * shiftBlue;

  // Set the left most updateLEDs with the new color
  for(int i = 0; i < updateLEDS; i++) {
    leds[i] = CRGB(nc.r, nc.g, nc.b);
    leds[NUM_LEDS-i-1] = CRGB(nc.r, nc.g, nc.b);
  }
  FastLED.show();
  delay(RefreshRate);
}
double convBrightness(int b) {
  double c = b/1024.0000;
  if( c < 0.3 ) {
    c = 0;
  }
  else if(c > 1) {
    c = 1.00;
  }
  return c;
}
