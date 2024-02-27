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

///String menu1[]={"3","setBeban","setJarak","setLed"};
String menu1[]={"3","kalibrasi beban","kalibrasi jarak","back"};
String menuJarak[]={"4","sensor 1","sensor 2","sensor 3","back"};
int currentLength;
int currentLayer =1;
int lastLayer=99;
int lastSubLayer=99;
int currentSelect=1;
int currentChange = 0;
int cursorLayer = 0;
int subLayer=0;
void setup() {
  
//  button0.attachDoubleClick(doubleClick);
Serial.begin(9600);
  lcd.begin ();
  //lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
button0.attachClick(singleClick);
}

void loop() {
  button0.tick();
  getRotary();
  showLcd();
}

void singleClick(){
  Serial.println("button run");
 if(currentLayer == 1){ 
  switch(currentSelect){
    case 1 :
      subLayer = 1;
      currentLayer = -1;
      currentSelect = 1;
      cursorSelect();
    break;

    case 2 :
      subLayer = 2;
      currentLayer =-1;
      currentSelect = 1;
     cursorSelect();
    break;
  };
 }
 //if(currentLayer == -1){
  
 
  //Serial.println(String() + "currentLayer:" + currentLayer);
}
void getRotary(){
  long newPosition = myEnc.read()/4;
  switch(currentLayer){
    case 1 :
      if(currentLayer != lastLayer){ clearMenu();}
      lastLayer = currentLayer;     
      currentLength = menu1[0].toInt();
    break;
  };
  switch(subLayer){
    case 1 :
      if(subLayer != lastSubLayer){ clearMenu();}
      lastSubLayer = subLayer;
      currentLength = 1;
     break;
     
    case 2 :
      if(subLayer != lastSubLayer){ clearMenu();}
      lastSubLayer = subLayer;
      currentLength = menuJarak[0].toInt();
     break;

     
  };
  
 if (newPosition != oldPosition){
  if (newPosition > oldPosition && currentSelect < currentLength) {
    clearSelect();
    currentSelect++;
    cursorSelect();
    
    
  }

  else if(newPosition < oldPosition && currentSelect != 1){
    clearSelect();
    currentSelect--;
    cursorSelect();
  }
  oldPosition = newPosition;
//  Serial.println(String() + "newPosition:" + newPosition);
  //Serial.println(String() + "currentSelect:" + currentSelect);
}
 Serial.println(String() + "currentSelect:" + currentSelect);
 Serial.println(String() + "cursorLayer:" + cursorLayer);
}

void showLcd(){
//  if(currentLayer==1){
//    lcd.setCursor(19,cursorLayer );
//    lcd.print("<");
//    
//    for(int i=0;i<currentLength;i++){
//      lcd.setCursor(0,i);
//      lcd.print(menu1[i+1]);
//    }
//  }
   if(currentLayer==1){
    lcd.setCursor(19,cursorLayer );
    lcd.print("<");

    for(int i=0;i<currentLength;i++){
      lcd.setCursor(0,i);
      lcd.print(menu1[i+1]);
    }
  }

   if(subLayer==1){
    lcd.setCursor(0,cursorLayer );
    lcd.print("test");
  }

   if(subLayer==2){
     lcd.setCursor(19,cursorLayer ); lcd.print("<");
     for(int i=0;i<currentLength;i++){
      lcd.setCursor(0,i);
      lcd.print(menuJarak[i+1]);
    }
  }

   
//  else if(currentLayer==5){
//    
//  }
  
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
  for(int i=0;i<20;i++){lcd.setCursor (i,line);}
  lcd.print(" ");
}

void clearMenu(){
  for(int i=0;i<20;i++){lcd.setCursor (i,0); lcd.print(" ");}
  
  for(int i=0;i<20;i++){lcd.setCursor (i,1); lcd.print(" ");}
  
  for(int i=0;i<20;i++){lcd.setCursor (i,2); lcd.print(" ");}
 
  for(int i=0;i<20;i++){lcd.setCursor (i,3); lcd.print(" ");}
 
}
