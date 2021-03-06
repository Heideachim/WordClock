
/*
    Ein WS2812B-Streifen ist an Pin D4 angeschlossen.
    An Pin D7 ist eine grüne LED zum Debuggen.
    Pin D2 ist der Digital-Eingang vom DCF77.
    
    Die Fastled-Bibliothek sperrt Interrupts, um das seriellen Protokoll
    zeitlich präzise zu erzeugen. Das stört aber die zeitbasierte Erkennung
    der DCF77-Bibliothek. Deshalb wird bis zum ersten Empfang eines gültigen
    Zeitsignales gar keine LEDs aktualisiert und dannach nur, wenn eine Hi->Low-Flanke
    erkannt wurde. Dies sollte der unkritischste Zeitpunkt für eine Interrupt-Sperre
    sein.   
    Die Anzeige wird nur jede Sekunde aktualisiert, um die Rechenzeit für die DCF77-Bibliothek
    zur Verfügung zu haben. Deshalb wird auch erst NACH einem fallenden Pegel die Uhrzeit
    ausgegeben, weil dann die DCF77-Routine erfolgreich abgeschlossen wurde.
    Wenn gar kein DCF77 empfangen wird, liegt ein konstanter Pegel am DCF77-Siganl an und
    es darf nicht auf den Flankenwechsel gewartet werden, sondern es muss nach einem Timeout
    die Zeit ausgegeben werden.
    
    Fehlersuche fehlende Synchronisation:
    Das Antennen-Ausgangs-Signal ist gelegentlich gestört, wodurch sich die Uhr nicht synchronisieren kann.
    Rot scheint nicht zu stören, Grün und vor allem Blau deutlich.
    Mit hue16 = 25209 gibt es sehr starke Störungen. Aber nur bei 0.00! Ist es 0.05 Uhr, geht es sehr gut.
    Die Störungen des dcf77 sind nicht korrelliert mit dem Ausgabesignal an die LEDs.
    Mit einem handy-netzteil scheint es besser zu sein als mit dem USBdes PCs.
    Verbesserungs-Test: Pro Streifen 2 mal 10µF eingelötet (also 120µF gesamt): Bringt keine merkliche Verbesserung
    Alle LEDs an mit der Farbe für hue=25209=grün (und zufälligerweise auch für farbe = 25209 = türkis) führt zu extremen Störungen.
    Sind die drei untersten Reihen auf der Farbe=25209, stört das überhaupt nicht, die vier untersten Reihen stören ganz leicht.
    Ist nur die oberste Reihe an, ist er teilweise extrem gestört!
    Ursache: Der obere LED-Streifen ist parallel und nahe bei der Antenne. De er in der Mitte eingespeist wird, fließt ein 
    hoher Strom an der Antenne vorbei und stört diese massiv. Je nach angezeigter Farbe sind die Stromrippel sehr unterschiedliche.
    Grundsätzlich zu sehen sind sehr hohe, sehr kurze Spitzen mit 20µs = 50kHz, unterlagert von einer niederfrequenten Welligkeit mit 1,1ms = 900Hz.
    */

#include <DCF77.h>    // DCF77 by Thijs Elenbaas, V1.0.0        https://github.com/thijse/Arduino-Libraries/downloads
                      //                                        https://github.com/thijse/Arduino-DCF77                                                                
#include <Time.h>     // Time by Michael Margolis, V1.6.0       http://www.arduino.cc/playground/Code/Time
#include <Timezone.h> // Timezone by Jack Christensen, V1.2.4   https://github.com/JChristensen/Timezone
#include "FastLED.h"  // FastLED by Daniel Garcia, V3.3.3       https://github.com/FastLED/FastLED

#define LED_DATA_PIN    4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS   96 //  60 LED/m, 6 lines á 16 LEDs
#define BRIGHTNESS  255
CRGB leds[NUM_LEDS];

#define DCF_PIN 2           // Connection pin to DCF 77 device
#define DCF_INTERRUPT 0    // Interrupt number associated with pin
DCF77 DCF = DCF77(DCF_PIN, DCF_INTERRUPT, false);

#define DEBUG_PIN 7       // green LED attached to pin 7


// =================================================================================================
void setup() {
  Serial.begin(115200);

  // initialize debug led:
  pinMode(DEBUG_PIN, OUTPUT);
  digitalWrite(DEBUG_PIN, HIGH);

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalPixelString)  //(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);
  FastLED.setBrightness(BRIGHTNESS);
  fill_solid(leds, NUM_LEDS, CRGB(3, 0, 0));
  leds[3] = CRGB(0, 100, 200);
  FastLED.show();  

  // initialize dcf77:
  DCF.Start();
  Serial.println("\n\nDCF77 benötigt über zwei Minuten, um sich zu synchronisieren!");
}

// =================================================================================================
void loop()
{  
  static time_t previous_display = 0;          // when the digital clock was displayed
  static bool previous_pinstate = 0;
  bool current_pinstate = digitalRead(DCF_PIN);
  digitalWrite(DEBUG_PIN, 1-current_pinstate);    // mirror the DCF77 line's state to a debug LED

  if (current_pinstate == 0)
    previous_pinstate = 0;

  time_t dcf_time = DCF.getTime(); // Check if new DCF77 time is available
  if (dcf_time != 0)
  {
    Serial.println("Synchronisiert");
    setTime(dcf_time);
  }

  // If there is no dcf77 Signal, the display routine gets never called, thus use a timeout to 
  // at least update the display every two seconds (1 sec = 51000 watchdog counts)
  static long watchdog;
  watchdog++;

  // update the display only if the time has changed AND the DCF77 signal was going from HI to LO OR if the watchdog has timed out:
  if ((now() != previous_display) && ((previous_pinstate == 0) && (current_pinstate == 1) || (watchdog > 90000L))) 
  {
    {
      previous_display = now();
      time_write_serial();  
      DCF.Stop();
      led_display(hour(), minute(), second());  
      delay(7);
      DCF.Start();
      previous_pinstate = 1;
      watchdog = 0;   // reset watchdog counter
    }
  }
}

// =================================================================================================

enum Zahlen        {esist, genau, fuenf, viertel, zwanzig, zehn, vor, nach, halb, gut, vier, fuenf_h, zehn_h, drei, ein, neun, sechs, zwei, elf, zwoelf, sieben, acht, eins, uhr, no};
int zahlen_pos[] = {0,     5,     9,     12,      27,      23,   20,  16,   32,   37,  41,   44,      59,     55,   52,  48,   64,    69,   73,  76,     91,     87,   84,   80, 999};
int zahlen_len[] = {5,     4,     3,     4,       5,       4,    3,   4,    5,    4,   3,    4,       5,      4,    3,   4,    5,     4,    3,   4,      5,      4,    3,    4,  999}; 

              //             0       5       10      15       20       25      30      35      40       45       50      55
enum Zahlen fuenfer[] = {       no,     fuenf,  zehn,   viertel, zwanzig, fuenf,  no,     fuenf,  zwanzig, viertel, zehn,   fuenf};
enum Zahlen wohin[] =   {       no,     nach,   nach,   nach,    nach,    vor,    no,     nach,   vor,     vor,     vor,    vor};
enum Zahlen halbe[] =   {       no,     no,     no,     no,      no,      halb,   halb,   halb,   no,      no,      no,     no};
int richtung[] =        {       0,      0,      0,      0,       0,       1,      1,      1,      1,       1,       1,      1};
enum Zahlen stunden[] = {no, eins, zwei, drei, vier, fuenf_h, sechs, sieben, acht, neun, zehn_h, elf, zwoelf, eins};  // ein zusätzlicher Eintrag, weil auch 12+1 abgefragt wird
enum Zahlen stunden_ein = ein;     // Sonderfall 'ein Uhr' (nicht 'eins Uhr')


void setled(int index, CRGB color)
{
  if (index < sizeof(zahlen_pos)) {
    int start = zahlen_pos[index];
    int len = zahlen_len[index];
    if (start < NUM_LEDS) {
      if (start + len <= NUM_LEDS)
        fill_solid(leds + zahlen_pos[index], len, color);
    }
  }
}

void led_display(int H, int M, int S) {
  int i;
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  int h = H;
  int m = (M+2)/5;                // Minuten auf Fünfminuten runden: 0, 1, 2 --> 0; 3, 4, 5 --> 5; etc. 
  if (M >= 58) {                   // Sonderfall: alles 2 Minuten vor einer ganzen Fünfminutenzahl wird auf diese aufgerundet, führt beo 58+59 zu einem Stunden-Überlauf!
      m = 0;
      h = H + 1;
  }

  while (h >= 13)                 // Nachmittags-Zeiten auf vormittags mappen
      h -= 12;           
  if (h == 0)                     // Sonderfall: Mitternacht wird als zwölf Uhr vereinfacht            
      h = 12;

  uint16_t hue16 = beat88(256);  // Sägezahn für die Farbe; 256 = 1bpm = 60s Periode; 2560 = 10bpm = 6s Periode
  CRGB farbe = CHSV( hue16 / 256, 255, 255);
  
  setled(esist, farbe);
  setled(fuenfer[m], farbe);
  setled(wohin[m], farbe);
  setled(halbe[m], farbe);
  if ((h == 1) && (m == 0))
      setled(stunden_ein, farbe);        // Sonderfall: 'ein Uhr' statt 'eins Uhr'
  else
      setled(stunden[h + richtung[m]], farbe);
  if (m == 0)                  // nur bei vollen Stunden wird 'Uhr' angehängt
      setled(uhr, farbe);
 
  FastLED.show();  
}

void time_write_serial(){
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
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
