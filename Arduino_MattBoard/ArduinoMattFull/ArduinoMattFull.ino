// Test Carte MATT
// Test CARTE SD + OLED + CODER + BOUTONS + LED

#include <Adafruit_NeoPixel.h>

//librairie graphique compacte pour OLEDS petits et grands
// https://github.com/olikraus/u8glib
#include <U8glib.h>  

// include SD library:
#include <SPI.h>
#include <SD.h>

// Pin de la led RGB
#define PIN            4
#define NUMPIXELS      1

// PB6 PIN 30         
#define B_MENU 5
// PE6 PIN 1 (int6)
#define encoder0PinA 7
// PC6 PIN31
#define encoder0PinB 6

#define BUT1 8
#define BUT2 9

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//PETIT OLED
//OLED I2C SSD1306
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);  // I2C      
// GRAND
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST); // Dev 0, Fast I2C / TWI

int r=0;
int g=0;
int b=0;

volatile  int encoder0Pos = 0;
int Lastencoder0Pos = 0;
uint32_t start_time;
int state=0;

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
bool card_ok=false;
uint32_t volumesize;

//SDCARD_SS_PIN
const int chipSelect = 17;

uint8_t l_bitmap[17];
//---------------------------------------------------------------------------------------------------
void setup_sdcard(){
  // 5 = vitesse, de 0 à 5 le plus lent
  if (!card.init(2, chipSelect)) {
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
      SD.begin(chipSelect);
  }
}

//---------------------------------------------------------------------------------------- 
// Fonctions de dessin
// un cadre
void u8g_frame() {
  //u8g.drawFrame(0,0,127,63);
  u8g.drawRFrame(0,0,127,63,5); //coins ronds
}
// un texte et une valeur  
void draw(String s, int v) {

  //u8g_frame( );
  u8g.setPrintPos(0,0);
  u8g.print(s);
  u8g.setScale2x2();  
  u8g.setPrintPos(14,9);
  if(v<100)  u8g.setPrintPos(30,9);
  if(v<10)  u8g.setPrintPos(46,9);
  u8g.print(v);
  u8g.undoScale();
  
}
// un texte au centre
void draw_msg(String s) {

  u8g_frame( );
  u8g.setPrintPos(26,22);
  u8g.print(s);

}

void draw_msg_card() {

  u8g_frame( );
  u8g.setPrintPos(5,0);
  u8g.print("SD CARD "+String(card.type()));
  u8g.setPrintPos(5,16);
  u8g.print("FAT"+String(volume.fatType()));
  u8g.setPrintPos(5,32);
  u8g.print(String(volumesize)+"MB");
}

void draw_color_val(int r,int g,int b) {

  u8g_frame( );
  u8g.setPrintPos(5,0);
  u8g.print("Rouge "+String(r));
  u8g.setPrintPos(5,15);
  u8g.print("Vert  "+String(g));
  u8g.setPrintPos(5,30);
  u8g.print("Bleu  "+String(b));
  u8g.setPrintPos(5,48);
  u8g.print(String(millis()/1000)+"s");
}



//---------------------------------------------------------------------------------------------------
// affiche une image, fichier texte hexa
void readfile_img(String imname){

  uint8_t c1,c2,k;
  u8g.firstPage(); 
  File dataFile = SD.open(imname);
  int j=0; // compteur de lignes
  int p=0; // compteur  de bloc oled
  if (dataFile) {
    do {
        for(int i=0;i<17;i++){ // lire une ligne + les caract fin de ligne
          c1=dataFile.read();
          if(c1==10 || c1==13 )c1=dataFile.read();
          c2=dataFile.read();
          // convertir d'hexa en décimal
          if(c1!=10 && c1!=13 && c2!=10 && c2!=13){
            c1=c1-48;if(c1>9)c1=c1-7;
            c2=c2-48;if(c2>9)c2=c2-7;
            k=c1*16+c2;
            l_bitmap[i]=k;
          }     
        }
        //for(int i=0;i<16;i++){Serial.print(l_bitmap[i],HEX);Serial.print(" ");}
        //Serial.println(); 
        //mettre une ligne dans l'écran
        u8g.drawBitmap( 0, j, 16, 1, l_bitmap); 
        j++;
        p++;if(p==8){p=0;u8g.nextPage();}
    } 
    while( j<64);
    
    dataFile.close();
  }
  else {
    Serial.println("error opening img.txt");
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

   // Nécessaire pour utiliser l'interruption 6
  // INT6
  EICRB |= (1<<ISC60)|(0<<ISC61); // sets the interrupt type CHANGE
  
   
  pinMode( chipSelect, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT); 

  pinMode(B_MENU, INPUT_PULLUP); // switch de l'encoder 
  pinMode(encoder0PinB, INPUT_PULLUP);  
  pinMode(encoder0PinA, INPUT);
  pinMode(BUT1, INPUT_PULLUP); 
  pinMode(BUT2, INPUT_PULLUP); 

  start_time=millis();
  
  Serial.begin(9600);
  // LEONARDO, attendre que la conction série soit établie
  while (!Serial && (digitalRead(BUT2)==HIGH));
  
  pixels.begin(); // This initializes the NeoPixel library.
  
  delay(1000);
  pixels.setPixelColor(0, pixels.Color(80,0,0)); 
  pixels.show();

  //u8g.setFont(u8g_font_unifont); //
  //u8g.setFont(u8g_font_cu12);
  u8g.setFont(u8g_font_profont15); //10 11 12 15 17 22 29
  //u8g.setFont(u8g_font_osb21); //times
  u8g.setColorIndex(1);  // pixel on     
  u8g.setFontPosTop(); // set upper left position for the string draw procedure
  u8g.firstPage(); do {draw_msg("READY"); } while( u8g.nextPage() );  

  Serial.println("Ready");
  delay(500);
  Serial.println("Initializing SD card...");
  setup_sdcard();

  u8g.firstPage(); do {draw_msg_card(); } while( u8g.nextPage() );  

  delay(2000);

  readfile_img("img1.txt");
  
 // u8g.firstPage(); do {draw_img(); } while( u8g.nextPage() ); 

  EIMSK |= (1<<INT6); // activates interrupt 6
  
}
//---------------------------------------------------------------------------------------- 
// interruption6, pulse de l'encoder   
ISR(INT6_vect) {
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    encoder0Pos++;delayMicroseconds(500);
  } else { encoder0Pos--;delayMicroseconds(500); }
}
//---------------------------------------------------------------------------------------------------

void loop() {

      if(digitalRead(B_MENU)== LOW) {
         state++;if (state>2)state=0;
         delay(50);
         while(digitalRead(B_MENU)== LOW);
         delay(200);
         }                      

      if(encoder0Pos!=Lastencoder0Pos){ // l'encodeur a tourné
        if(encoder0Pos>Lastencoder0Pos){
            if(state==0){ if(r<255)r++;  }
            if(state==1){ if(g<255)g++;  }
            if(state==2){ if(b<255)b++;  }
        }   else {
            if(state==0){ if(r>0)r--;  }
            if(state==1){ if(g>0)g--;  }
            if(state==2){ if(b>0)b--;  }
            
        }
        u8g.firstPage(); do { draw_color_val(r,g,b); } while( u8g.nextPage() );
        Lastencoder0Pos=encoder0Pos;
      }
      
      pixels.setPixelColor(0, pixels.Color(r,g,b)); 
      pixels.show();

      if(digitalRead(BUT1)== LOW) {
        r=0;g=0;b=0;
        Serial.println("BP1");
        delay(20);
        while(digitalRead(BUT1)== LOW);
      }

      if(digitalRead(BUT2)== LOW) {
        long st=millis();
        readfile_img("img.txt");
        Serial.println(millis()-st);
        Serial.println("BP2");
        delay(20);
        while(digitalRead(BUT2)== LOW);
      }


 

}
