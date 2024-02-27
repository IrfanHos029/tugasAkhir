#include <OneButton.h>
#include <Encoder.h>
#include <Wire.h>
//#include <LCD.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C  lcd(0x27,20,4);

long oldPosition  = 0;
int encoderDTpin = 2;
int encoderCLKpin = 3;
Encoder myEnc(encoderDTpin, encoderCLKpin);

int buttonPin = 8;
OneButton button0(buttonPin, true);

String menu1[]={"setBeban","setJarak","setLed"};
String menu2[]={"test 1","test 2","test 3"};
int currentLength = 3;
int currentLayer =0;
int lastLayer=99;
int currentSelect=1;

int cursorLayer = 0;
void setup() {
//  button0.attachClick(singleClick);
//  button0.attachDoubleClick(doubleClick);
Serial.begin(9600);
  lcd.begin ();
  //lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);

}

void loop() {
  getRotary();
  showLcd();
}

void getRotary(){
  long newPosition = myEnc.read()/4;
 if (newPosition != oldPosition){
  if (newPosition != oldPosition && newPosition > currentLength) {
    clearSelect();
    currentSelect++;
    cursorSelect();
    Serial.println(newPosition);
  }

  else if(newPosition != oldPosition && currentSelect < currentLength){
    clearSelect();
    currentSelect--;
    cursorSelect();
  }
  oldPosition = newPosition;
  Serial.println(String() + "newPosition:" + newPosition);
  Serial.println(String() + "currentSelect:" + currentSelect);
}
}

void showLcd(){
  lcd.setCursor(19,cursorLayer );
  lcd.print("<");
 
  for(int i=0;i<3;i++){
    lcd.setCursor(0,i);
    lcd.print(menu1[i]);
  }
   
  
}

void cursorSelect(){
  switch (currentSelect){
          case 1:
            cursorLayer = 0;
            break;
          case 2:
            cursorLayer = 1;
            break;
          case 3:
            cursorLayer = 2;
            break;
          case 4:
            cursorLayer = 3;
            break;
        };
}

void clearSelect(){
  lcd.setCursor (19,0);
  lcd.print(" ");
  lcd.setCursor (19,1);
  lcd.print(" ");
  lcd.setCursor (19,2);
  lcd.print(" ");
  lcd.setCursor (19,3);
  lcd.print(" ");
}

void clearChar(int charPosition, int line){
  lcd.setCursor (charPosition,line);
  lcd.print(" ");
}

void clearLine(int line){
  lcd.setCursor (0,line);
  lcd.print("                ");
}

void clearMenu(){
  lcd.setCursor (1,0);
  lcd.print("               ");
  lcd.setCursor (1,1);
  lcd.print("               ");
}
