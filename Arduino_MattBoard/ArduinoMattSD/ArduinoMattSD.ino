// Test Carte MATT
// Test CARTE SD

#include <Adafruit_NeoPixel.h>

// include SD library:
#include <SPI.h>
#include <SD.h>

// Pin de la led RGB
#define PIN            4
#define NUMPIXELS      1

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
bool card_ok=false;

//SDCARD_SS_PIN
const int chipSelect = 17;
//---------------------------------------------------------------------------------------------------
void setup_sdcard(){
  // 5 = vitesse, de 0 à 5 le plus lent
  if (!card.init(5, chipSelect)) {
    Serial.println("Initialization failed.");
  } else {
    pixels.setPixelColor(0, pixels.Color(0,80,0)); pixels.show();
    card_ok=true;
  }
  delay(100);
  if(card_ok){
    // print the type of card
    Serial.println();
    Serial.print("Card type:         ");
    switch (card.type()) {
      case SD_CARD_TYPE_SD1:
        Serial.println("SD1");
        break;
      case SD_CARD_TYPE_SD2:
        Serial.println("SD2");
        break;
      case SD_CARD_TYPE_SDHC:
        Serial.println("SDHC");
        break;
      default:
        Serial.println(card.type());
    }
  
    // open the 'volume'/'partition' - it should be FAT16 or FAT32
    if (!volume.init(card)) {
      Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
      card_ok=false;
    }
  }
  if(card_ok){
  
      uint32_t volumesize;
      Serial.print("Volume type is:    FAT");
      Serial.println(volume.fatType(), DEC);
      volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
      volumesize *= volume.clusterCount();       // we'll have a lot of clusters
      volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
      volumesize /= 1024;
      Serial.print("Volume size (Mb):  ");
      Serial.println(volumesize);    
      Serial.println("\nFiles: ");
      root.openRoot(volume);
    
      // list all files in the card with date and size
      root.ls(LS_R | LS_DATE | LS_SIZE);
  }
}
//---------------------------------------------------------------------------------------------------

void setup() {

 // LEONARDO
 // Configure le Clock Divider à 1 pour 16 MHz avec quatz 16Mhz
 // C'est normalement fait dans le bootloader, mais ici, pas de bootloader
 // CLKPR=128;
 // CLKPR=0;
 // Desactive le port JTAG pour utiliser le PORTF
   MCUCR = (1 << JTD) | (1 << IVCE) | (0 << PUD);
   MCUCR = (1 << JTD) | (0 << IVSEL) | (0 << IVCE) | (0 << PUD);
   
 // LED RX partagée avec le CS de la carte SD, 
 // c'est censé la désactiver ou simplement l'éteindre ?
   RXLED0;
   
  pinMode( chipSelect, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT); 
  
  Serial.begin(9600);
  // LEONARDO, attendre que la conction série soit établie
  while (!Serial);
  
  pixels.begin(); // This initializes the NeoPixel library.
  
  delay(1000);
  pixels.setPixelColor(0, pixels.Color(80,0,0)); 
  pixels.show();

  Serial.println("Ready");
  Serial.println("Initializing SD card...");
  setup_sdcard();
  
}
//---------------------------------------------------------------------------------------------------

void loop() {

}
