// CARTE MATTI ATMEGA32U4 / LEONARDO
// ECRAN OLED I2C
// ENCODER ROTATIF

#include <U8glib.h>     
#include <Adafruit_NeoPixel.h>

// PB6 PIN 30         
#define B_MENU 5
// PE6 PIN 1 (int6)
#define encoder0PinA 7
// PC6 PIN31
#define encoder0PinB 6

#define BUT1 8
#define BUT2 9

#define PIN            4
#define NUMPIXELS      1

// RXLED D17
// TXLED D30


int r=0;
int g=0;
int b=0;

volatile  int encoder0Pos = 0;
int Lastencoder0Pos = 0;

int state=0;

long pushtime=0;

//PETIT OLED
//OLED I2C SSD1306
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);  // I2C      
// GRAND
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST); // Dev 0, Fast I2C / TWI

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//----------------------------------------------------------------------------------------
void setup() 
{
  // Si pas de bootloader Leonardo
  // Desactive le port JTAG pour utiliser le PORTF A0-A3
  //  MCUCR = (1 << JTD) | (1 << IVCE) | (0 << PUD);
  //  MCUCR = (1 << JTD) | (0 << IVSEL) | (0 << IVCE) | (0 << PUD);
  // Configure le Clock Divider à 1 pour 16 MHz avec quartz 16Mhz
  //  CLKPR=128;
  //  CLKPR=0;

  // Nécessaire pour utiliser l'interruption 6
  // INT6
  EICRB |= (1<<ISC60)|(0<<ISC61); // sets the interrupt type CHANGE
     
  pinMode(B_MENU, INPUT_PULLUP); // switch de l'encoder 
  pinMode(encoder0PinB, INPUT_PULLUP);  
  pinMode(encoder0PinA, INPUT);
  pinMode(BUT1, INPUT_PULLUP); 
  pinMode(BUT2, INPUT_PULLUP); 
   
  //while (!Serial);
  Serial.begin(9600);
  Serial1.begin(9600);
  pixels.begin(); // This initializes the NeoPixel library.

  delay(250);
 
  Serial.println("OLED CTRL");

  //u8g.setFont(u8g_font_unifont); //
  //u8g.setFont(u8g_font_cu12);
  u8g.setFont(u8g_font_profont29); //10 11 12 15 17 22 29
  //u8g.setFont(u8g_font_osb21); //times
  u8g.setColorIndex(1);  // pixel on     
  u8g.setFontPosTop(); // set upper left position for the string draw procedure
    
  EIMSK |= (1<<INT6); // activates interrupt 6
}

//---------------------------------------------------------------------------------------- 
// interruption6, pulse de l'encoder   
ISR(INT6_vect) {
  if (digitalRead(encoder0PinA) == digitalRead(encoder0PinB)) {
    encoder0Pos++;delayMicroseconds(500);
  } else { encoder0Pos--;delayMicroseconds(500); }
}

//---------------------------------------------------------------------------------------- 
// Fonctions de dessin
// un cadre
void u8g_frame() {
  //u8g.drawFrame(0,0,127,63);
  u8g.drawRFrame(0,0,127,63,5); //coins ronds
}
// un texte   
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

void draw_msg(String s) {

  u8g_frame( );
  u8g.setPrintPos(50,18);
  u8g.print(s);

}


//----------------------------------------------------------------------------------------

void loop()
{
     
      if(state==0){u8g.firstPage(); do { draw("RED",r); } while( u8g.nextPage() );}
      if(state==1){u8g.firstPage(); do { draw("GREEN",g); } while( u8g.nextPage() );}
      if(state==2){u8g.firstPage(); do { draw("BLUE",b); } while( u8g.nextPage() );}
          
      if(digitalRead(B_MENU)== LOW) {
         state++;if (state>2)state=0;
         delay(50);
         while(digitalRead(B_MENU)== LOW);
         delay(200);
         }                      

      if(encoder0Pos!=Lastencoder0Pos){
        if(encoder0Pos<0)encoder0Pos=0;
        if(encoder0Pos>255)encoder0Pos=255;
        if(state==0){ r=encoder0Pos;  }
        if(state==1){ g=encoder0Pos;  }
        if(state==2){ b=encoder0Pos;  }     
        Lastencoder0Pos=encoder0Pos;
      }
      
      pixels.setPixelColor(0, pixels.Color(r,g,b)); 
      pixels.show();
          
 }
