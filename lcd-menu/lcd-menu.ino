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
String menu1[]={"3","kalibrasi Beban","Kalibrasi Jarak","Back"};
String menuJarak[]={"4","Sensor 1:","Sensor 2:","Sensor 3:","Back"};
int currentLength;
int currentLayer =0;
int lastLayer=99;
int lastSubLayer=99;
int currentSelect=1;
int currentChange = 0;
int cursorLayer = 0;
int subLayer=0;
float parWeight=900;
int valueLength = 49;
int valueWidth = 49;
int valueHeight = 49;
int flagS=0;
long newPosition=0;


void setup() {
  
//  button0.attachDoubleClick(doubleClick);
Serial.begin(9600);
  lcd.begin ();
  //lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
button0.attachClick(singleClick);
button0.attachDoubleClick(doubleclick1);

}

void loop() {
  button0.tick();
  getRotary();
  showLcd();

}

void singleClick(){
 // getRotary();
  Serial.println("button 1 klik run");
 if(currentLayer == 1 && subLayer == 0){ 
  switch(currentSelect){
    case 1 :
      clearMenu();
      subLayer = 1;
      currentLayer = -1;
      currentSelect = 1;
      cursorSelect();
    break;

    case 2 :
      clearMenu();
      subLayer = 2;
      currentLayer =-1;
      currentSelect = 1;
      flagS = 1;
     cursorSelect();
    break;

    case 3 :
      clearMenu();
      
      currentLayer = 0;
      
    break;
  };
 }
 
  else if(currentLayer != 1 && subLayer == 1){
  clearMenu();
  
  subLayer = 0;
  currentLayer = 1;
  currentSelect = 1;
  cursorSelect();
 }

  if(currentLayer != 1 && subLayer == 2 ){
    
    switch(currentSelect){
    case 1 :
      flagS=!flagS;
      clearMenu();
      //Serial.println(String() + "falg run 1:" + flagS);
    break;

    case 2 :
      flagS=!flagS;
      clearMenu();
      //Serial.println(String() + "falg run 2:" + flagS);
    break;

    case 3 :
     flagS=!flagS;
     clearMenu();
     //Serial.println(String() + "falg run 3:" + flagS);
    break;

   case 4 :
      clearMenu();
      subLayer = 0;
      currentLayer = 1;
      currentSelect = 2;
      flagS = 0;
      cursorSelect();
    break;

    
  };
   }
//   else if(currentLayer != 1 && subLayer == 3 ){
//     currentLayer = 0;
//   }

   
//if(currentLayer != 1 && subLayer == 2 && flagS != 0){
//  switch(flagS){
//    case 1 :
//      flagS=0;
//      Serial.println(String() + "falg run 2:" + flagS);
//    break;
//
//    case 2 :
//      flagS=0;
//      //currentLayer++;
//    break;
//
//    case 3 :
//
//    break;
//  };
//  
//  }
//  Serial.println(String() + "currentSelect:" + currentSelect);
//  Serial.println(String() + "valueLength:" + valueLength);
//  Serial.println(String() + "currentLayer:" + currentLayer);
//  Serial.println(String() + "falg run 1:" + flagS);
}

void doubleclick1(){
  Serial.println("button 2 klik run");
  currentLayer = 1;
  currentSelect = 1;
  cursorSelect();
  clearMenu();
  }

void getRotary(){
   newPosition = myEnc.read()/4;
  switch(currentLayer){
    case 1 :
     // if(currentLayer != lastLayer){ clearMenu();}
      lastLayer = currentLayer;     
      currentLength = menu1[0].toInt();
    break;
  };
  switch(subLayer){
    case 1 :
     // if(subLayer != lastSubLayer){ clearMenu();}
      lastSubLayer = subLayer;
      currentLength = 1;
     break;
     
    case 2 :
     // if(subLayer != lastSubLayer){ clearMenu();}
      lastSubLayer = subLayer;
      currentLength = menuJarak[0].toInt();
     break;

     case 3 :
       lastSubLayer = subLayer;
       currentLength = 1;
     break;

     
  };
  
 if (newPosition != oldPosition){
  if (newPosition > oldPosition && currentSelect < currentLength && flagS == 0) {
    clearSelect();
    currentSelect++;
    cursorSelect(); 
  }

  else if(newPosition < oldPosition && currentSelect != 1 && flagS == 0){
    clearSelect();
    currentSelect--;
    cursorSelect();
  }
  
//  Serial.println(String() + "newPosition:" + newPosition);
  //Serial.println(String() + "currentSelect:" + currentSelect);
  if(subLayer==1){
     if(newPosition > oldPosition && parWeight < 1000) {
     parWeight++;
     }
     else if(newPosition < oldPosition && parWeight != 0){
      parWeight--;
     }
  }

   if(subLayer==2 && currentSelect == 1 && flagS == 1){
    if(newPosition > oldPosition && valueLength < 50) {
     valueLength++;
     }
     else if(newPosition < oldPosition && valueLength > -1){
      valueLength--;
     }  
  }

  if(subLayer==2 && currentSelect == 2 && flagS == 1){
    if(newPosition > oldPosition && valueWidth < 50) {
     valueWidth++;
     }
     else if(newPosition < oldPosition && valueWidth > -1){
      valueWidth--;
     }  
  }

  if(subLayer==2 && currentSelect == 3 && flagS == 1){
    if(newPosition > oldPosition && valueHeight < 50) {
     valueHeight++;
     }
     else if(newPosition < oldPosition && valueHeight > -1){
      valueHeight--;
     }  
  }
  oldPosition = newPosition;

}

// Serial.println(String() + "currentSelect:" + currentSelect);
// Serial.println(String() + "cursorLayer:" + cursorLayer);
}

void showLcd(){
  int dataSensor[]={valueLength,valueWidth,valueHeight,0};
  if(currentLayer==0){
    lcd.setCursor(0,0);
    lcd.print("Panjang:50");
    //lcd.print((hasilP >= 50)? 0 : hasilP);
    lcd.setCursor(13,0);
    lcd.print(" cm");
    
    lcd.setCursor(0,1);
    lcd.print("Lebar  :50");
    //lcd.print((hasilL >= 50)? 0 : hasilL);
    lcd.setCursor(13,1);
    lcd.print(" cm");
    
    lcd.setCursor(0,2);
    lcd.print("Tinggi :50");
    //lcd.print((hasilT >= 50)? 0 : hasilT);
    lcd.setCursor(13,2);
    lcd.print(" cm");

    lcd.setCursor(0,3);
    lcd.print("Berat  :500");
    //lcd.print((weight >= 1000)? weight / 1000 :  weight);
    lcd.setCursor(13,3);
    lcd.print("Gram");
    
   
  }
   if(currentLayer==1){
    lcd.setCursor(18,cursorLayer );
    lcd.print("<-");

    for(int i=0;i<currentLength;i++){
      lcd.setCursor(0,i);
      lcd.print(menu1[i+1]);
    }
  }

   if(subLayer==1){
    lcd.setCursor(0,0);
    lcd.print("BERAT:");
    lcd.setCursor(6,1);
    lcd.print(" 0.00 Gram");
    lcd.setCursor(0,3);
    lcd.print("Parameter : ");
    lcd.print(parWeight);
  }

   if(subLayer==2){
     lcd.setCursor(18,cursorLayer ); lcd.print("<-");
     
     for(int i=0;i<currentLength;i++){
      lcd.setCursor(0,i);
      lcd.print(menuJarak[i+1]);  
      lcd.setCursor(10,(i==3)?2:i);
      lcd.print(dataSensor[(i==3)?2:i]); 
      lcd.setCursor(13,(i==3)?2:i);
      lcd.print(" CM");
    }
    if(flagS){lcd.setCursor(12,cursorLayer); lcd.print("*");}
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
  lcd.setCursor (18,0);
  lcd.print("  ");
  lcd.setCursor (18,1);
  lcd.print("  ");
  lcd.setCursor (18,2);
  lcd.print("  ");
  lcd.setCursor (18,3);
  lcd.print("  ");
}

void clearChar(int charPosition, int line){
  lcd.setCursor (charPosition,line);
  lcd.print(" ");
}

void clearLine(int line){
  for(int i=0;i<20;i++){lcd.setCursor (i,line); lcd.print(" ");}
  
}

void clearMenu(){
  for(int i=0;i<20;i++){lcd.setCursor (i,0); lcd.print(" ");}
  
  for(int i=0;i<20;i++){lcd.setCursor (i,1); lcd.print(" ");}
  
  for(int i=0;i<20;i++){lcd.setCursor (i,2); lcd.print(" ");}
 
  for(int i=0;i<20;i++){lcd.setCursor (i,3); lcd.print(" ");}
 
}

//void activeSelect(int row,int colom){
//  unsigned long tmr = millis();
//  static long saveTmrBlink=0;
//  static bool state=false;
//  
//  if(tmr - saveTmrBlink >= 1000){
//    saveTmrBlink = tmr;
//    state=!state;
//  //lcd.blink();
//  lcd.setCursor(row,colom);
//  if(state){lcd.write(byte(0)); lcd.write(byte(0));}
//  else{lcd.print("  ");}
//  Serial.println(String() + "cursorLayer:" + cursorLayer);
//  }
//  
//}
//
//void inActiveSelect(int row,int colom){
//  lcd.setCursor(row,colom);
//  lcd.noBlink();
//}
