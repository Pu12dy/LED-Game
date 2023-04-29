#include <LiquidCrystal.h>
#include <SPI.h>
using namespace std;

//*****************  LCD Set-Up *********************************//
// Output pin variables, set to the pins being used by the LCD
const int rs = 4,en = 5, d4 = 6, d5 = 7, d6 = 8, d7 =9;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//*****************  SPI Set-Up *********************************//
char buff[255];
volatile byte indx;
volatile boolean process;

//*****************  Buzzzer Set-Up *********************************//
const int buzzer = 3;
unsigned long lastRan;
const unsigned long period = 250;
unsigned char c,t;
const unsigned char SONG_LENGTH = 20;                          
const double Sarias_Song[SONG_LENGTH] = {349.23, 440.00, 493.88, 349.23, 440.00, 493.88, 349.23, 440.00, 493.88,
                                                  659.25, 587.33, 493.88, 523.25, 493.88, 392.00, 329.63, 293.66, 329.63, 
                                                  392.00, 329.63};                              
const unsigned char noteDuration[SONG_LENGTH] = {2, 1, 2, 1, 1, 2, 1, 1, 1,
                                                 1, 2, 1, 1, 1, 1, 2, 1, 1,
                                                 1, 2};

void newGame()
{
  lcd.clear();
  lcd.print("Players Scores:");
  lcd.setCursor(0,1);
  lcd.print("A: 0   B: 0");
}

void setup() {
  lastRan = 0; 
  c = 0;
  t = 0;
  Serial.begin(115200);
  pinMode(MISO, OUTPUT); // have to send on master in so it set as output
  SPCR |= _BV(SPE);// turn on SPI in slave mode
  indx = 0; // buffer empty
  process = false;
  SPI.attachInterrupt(); // turn on interrupt
  lcd.begin(16,2);
  newGame();
}

ISR (SPI_STC_vect) // SPI interrupt routine 
{ 
   byte c = SPDR; // read byte from SPI Data Register
   
   if (indx < sizeof(buff)) {
      buff[indx++] = c; // save data in the next index in the array buff
      if (c == '\n') { 
        buff[indx - 1] = 0; // replace newline ('\n') with end of string (0)
        process = true;
      }
   }   
}

void updateLCD(char buff[255])
{
  lcd.clear();
  lcd.print("Players Scores:");
  lcd.setCursor(0,1);
  lcd.print(buff);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (process) {
      process = false; //reset the process
      updateLCD(buff); //print the array on LCD
      indx= 0; //reset button to zero
      Serial.print(buff);
   }

   if((millis() - lastRan) > period)
   {
   if(c == 20)
      {
        c = 0;
      }
      if(t == noteDuration[c] - 1)
      {
        c++;
        t = 0;
      }
      else
      {
        t++;
      }
   tone(buzzer, Sarias_Song[c]);
   lastRan = millis();
   }
}
