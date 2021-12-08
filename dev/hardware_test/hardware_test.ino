/*
    An Pin D7 ist eine grüne LED zum Debuggen.
 */

#include <DCF77.h>    // DCF77 by Thijs Elenbaas, V1.0.0        https://github.com/thijse/Arduino-Libraries/downloads
                      //                                        https://github.com/thijse/Arduino-DCF77                                                                
#include <Time.h>     // Time by Michael Margolis, V1.6.0       http://www.arduino.cc/playground/Code/Time
#include <Timezone.h> // Timezone by Jack Christensen, V1.2.4   https://github.com/JChristensen/Timezone
#include "FastLED.h"  // FastLED by Daniel Garcia, V3.3.3       https://github.com/FastLED/FastLED

#define LED_DATA_PIN    4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    120 //  60 LED/m * 2m
#define BRIGHTNESS  255
CRGB leds[NUM_LEDS];

#define DCF_PIN 2	         // Connection pin to DCF 77 device
#define DCF_INTERRUPT 0		 // Interrupt number associated with pin
time_t prevDisplay = 0;          // when the digital clock was displayed
time_t time;
DCF77 DCF = DCF77(DCF_PIN, DCF_INTERRUPT);

#define DEBUG_PIN 7       // green LED attached to pin 7


void setup() {
  Serial.begin(115200);

  pinMode(DEBUG_PIN, OUTPUT);
  digitalWrite(DEBUG_PIN, LOW);

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalPixelString)  //(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  FastLED.setBrightness(BRIGHTNESS);

  DCF.Start();
  setSyncInterval(30);
  setSyncProvider(getDCFTime);
  Serial.println("\n\nDCF77 benötigt über zwei Minuten, um sich zu synchronisieren.");
}


void loop()
{  
  digitalWrite(DEBUG_PIN, digitalRead(DCF_PIN));

  if( now() != prevDisplay) //update the display only if the time has changed
  {
    prevDisplay = now();
    digitalClockDisplay();  
    led_display();
  }
}

void led_display(void) {
  int i;
  fill_solid(leds, NUM_LEDS, CRGB(0, 0, 2));
  for (i=0; i < second(); i++) {
      leds[i] = CRGB(4, 10, 2);
  }
  FastLED.show();  
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  //printDigits(minute());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(".");
  Serial.print(month());
  Serial.print(".");
  Serial.print(year()); 
    if (timeStatus()== timeNotSet)
       Serial.print(" ?");
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

unsigned long getDCFTime()
{ 
  time_t DCFtime = DCF.getTime(); 
  if (DCFtime!=0) {
    Serial.println("Synchronisiert.");    // Indicator that a time check is done
  }
  return DCFtime;
}
