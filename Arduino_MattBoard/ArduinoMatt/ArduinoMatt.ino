// Test Carte MATT

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// include the SD library:
#include <SPI.h>
#include <SD.h>

#define PIN            4
#define NUMPIXELS      1

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 10; 

void setup() {

 // Configure le Clock Divider à 1 pour 16 MHz avec quatz 16Mhz
 // C'est normalement fait dans le bootloader, mais ici, pas de bootloader
  CLKPR=128;
  CLKPR=0;
  // Desactive le port JTAG pour utiliser le PORTF
   MCUCR = (1 << JTD) | (1 << IVCE) | (0 << PUD);
   MCUCR = (1 << JTD) | (0 << IVSEL) | (0 << IVCE) | (0 << PUD);

  Serial.begin(9600);
  pixels.begin(); // This initializes the NeoPixel library.
  delay(2000);
  Serial.println("Ready");

}

void loop() {
int i;
    for(i=0;i<255;i=i+3){
      pixels.setPixelColor(0, pixels.Color(0,i,0)); 
      pixels.show();
      delay(delayval); // Delay for a period of time (in milliseconds).
    }
    for(i=0;i<255;i=i+3){
      pixels.setPixelColor(0, pixels.Color(i,0,0)); 
      pixels.show();
      delay(delayval); // Delay for a period of time (in milliseconds).
    }
    for(i=0;i<255;i=i+3){
      pixels.setPixelColor(0, pixels.Color(0,0,i)); 
      pixels.show();
      delay(delayval); // Delay for a period of time (in milliseconds).
    }
    for(i=0;i<255;i=i+3){
      pixels.setPixelColor(0, pixels.Color(i,0,i)); 
      pixels.show();
      delay(delayval); // Delay for a period of time (in milliseconds).
    } 
    Serial.println("Next");
}
