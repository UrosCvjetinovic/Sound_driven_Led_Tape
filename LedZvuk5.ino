#include "FastLED.h"

#define NUM_LEDS 300        // Velicina koriscene trake
#define updateLEDS 1        // Koliko dioda se updejtuje po sekundi
#define COLOR_SHIFT 1000  // Vreme izmedju promene boje (u ms)
CRGB leds[NUM_LEDS];        // niz led dioda koji obradjujemo
#define minColorShift 50
#define minColorDif   20
#define maxColorShift 200
#define maxColorDif   50
#define ShiftColorLoop 1000
#define BrightnessScale 0.3


#define RefreshRate 0        // Koliko cesto se cita zvuk (u ms)
// Input/Output
#define DATA_PIN 6          // Izlaz koji kontrolise diode
#define BRIGHT_PIN 0         // audio senzor(mikrofon)

// Unutrasnje promenljive

int shiftRed = 0;
int shiftGreen = 0;
int shiftBlue = 0;
long shiftColorTime = 0;
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

  for (int i = 0; i < NUM_LEDS ; i++) { //Ugasi sve diode
    leds[i] = CRGB(0, 0, 0);
  }
  randomSeed(analogRead(3));
  FastLED.show();
}

void loop() {
  unsigned long time1 = millis();
  //Serial.println(time);
  long time2 = time1 * random(10);
  long time3 = time1 * random(10);
  Serial.println(shiftColorTime);
  
  if ( shiftColorTime > 3 * ShiftColorLoop) {
      shiftColorTime = 0;
  }
  else if ( shiftColorTime >= 2 * ShiftColorLoop) {     //dominatna plava
    shiftRed = time1 % minColorShift + random(minColorDif);
    shiftGreen = time2 % minColorShift + random(minColorDif);
    shiftBlue = time3 % maxColorShift + random(maxColorDif);
    shiftColorTime = shiftColorTime + 1;
  }
  else if ( shiftColorTime >= ShiftColorLoop ) {      //dominatna zelena
    shiftRed = time1 % minColorShift + random(minColorDif);
    shiftGreen = time2 % maxColorShift + random(maxColorDif);
    shiftBlue = time3 % minColorShift + random(minColorDif);
    shiftColorTime = shiftColorTime + 1;

  }
  else if ( shiftColorTime < ShiftColorLoop ) {           //dominatna crvena
    shiftRed = time1 % maxColorShift + random(maxColorDif);
    shiftGreen = time2 % minColorShift + random(minColorDif);
    shiftBlue = time3 % minColorShift + random(minColorDif);
    shiftColorTime = shiftColorTime + 1;
  }
  


  // Shift all LEDs to the right by updateLEDS number each time
  for (int i = NUM_LEDS - 1; i >= updateLEDS; i--) {
    leds[i] = leds[i - updateLEDS];
  }

  // Get the pitch and brightness to compute the new color
  Color nc;
  int s;
  s = analogRead(BRIGHT_PIN);
  double bright = convBrightness(s);
  bright = bright * BrightnessScale;

  nc.r = bright * shiftRed;
  nc.g = bright * shiftGreen;
  nc.b = bright * shiftBlue;

  // Set the left most updateLEDs with the new color
  for (int i = 0; i < updateLEDS; i++) {
    leds[i] = CRGB(nc.r, nc.g, nc.b);
  }
  FastLED.show();
  delay(RefreshRate);
}
double convBrightness(int b) {
  double c = b / 1024.0000;
  if ( c < 0.3 ) {
    c = 0;
  }
  else if (c > 1) {
    c = 1.00;
  }
  return c;
}
