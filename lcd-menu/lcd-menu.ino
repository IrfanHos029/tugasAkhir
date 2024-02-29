#include <OneButton.h>
#include <Encoder.h>
#include <Wire.h>
//#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C  lcd(0x27,20,4);

long oldPosition  = 0;
int encoderDTpin = 2;
int encoderCLKpin = 3;
Encoder myEnc(encoderDTpin, encoderCLKpin);

int buttonPin = 8;
OneButton button0(buttonPin, true);

///String menu1[]={"3","setBeban","setJarak","setLed"};
String menu1[]={"6","kalibrasi Beban","Kalibrasi Jarak","Set Timer LCD","Set Timer Sleep","Set LED","Back"};
String menuJarak[]={"4","Sensor 1:","Sensor 2:","Sensor 3:","Back"};
int led[]={8,9,10};
int currentLength;
int currentLayer =0;
int lastLayer=99;
int lastSubLayer=99;
int currentSelect=1;
int currentChange = 0;
int cursorLayer = 0;
int subLayer=0;
float parWeight=900;
int valueLength = 0;
int valueWidth = 0;
int valueHeight = 0;
int flagS=0;
long newPosition=0;
int stepLayer=1;
int lastStep,lastSleep;
int timerLock,timerSleep;
char *panah[]{" ","<",">"};
int panahRun,lastLock;
int merah,biru,hijau;
/*
 * address  val
 * 0        weight
 * 1        sensor 1
 * 2        sensor 2
 * 3        sensor 3
 */
void setup() {
  
//  button0.attachDoubleClick(doubleClick);
Serial.begin(9600);
  lcd.begin ();
  //lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
button0.attachClick(singleClick);
button0.attachDoubleClick(doubleclick1);
for(int i=0;i<3;i++){ pinMode(led[i],OUTPUT);}
parWeight = EEPROM.read(0);
valueLength = EEPROM.read(1);
valueWidth = EEPROM.read(2);
valueHeight = EEPROM.read(3);
timerLock =  EEPROM.read(4);
timerSleep =EEPROM.read(5);
delay(100);
Serial.println(String() + "parWeight  :" + parWeight);
Serial.println(String() + "valueLength:" + valueLength);
Serial.println(String() + "valueWidth :" + valueWidth);
Serial.println(String() + "valueHeight:" + valueHeight);
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
      subLayer = 3;
      currentLayer = -1;
      currentSelect = 1;
      cursorSelect();
    break;

    case 4 :

      clearMenu();
      subLayer = 4;
      currentLayer = -1;
      currentSelect = 1;
      cursorSelect();
    break;

    case 5 :
      clearMenu();
      stepLayer = 1;
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
  EEPROM.write(0,parWeight);
  //EEPROM.commit();
 }

 else if(currentLayer != 1 && subLayer == 3 ){
      clearMenu();
      subLayer = 0;
      currentLayer = 1;
      currentSelect = 3;
      cursorSelect();
      EEPROM.write(4,timerLock);
    }

  else if(currentLayer != 1 && subLayer == 4 ){//
      clearMenu();
      subLayer = 0;
      currentLayer = 1;
      currentSelect = 4;
      cursorSelect();
      EEPROM.write(5,timerSleep);
    }


  if(currentLayer != 1 && subLayer == 2 ){
    
    switch(currentSelect){
    case 1 :
      flagS=!flagS;
      clearMenu();
    break;

    case 2 :
      flagS=!flagS;
      clearMenu();
    break;

    case 3 :
     flagS=!flagS;
     clearMenu();
    break;

   case 4 :
      clearMenu();
      subLayer = 0;
      currentLayer = 1;
      currentSelect = 2;
      flagS = 0;
      cursorSelect();
      EEPROM.write(1,valueLength);
      EEPROM.write(2,valueWidth);
      EEPROM.write(3,valueHeight);
    break;

    
  };
   }

    

//  Serial.println(String() + "currentSelect:" + currentSelect);
//  Serial.println(String() + "valueLength:" + valueLength);
//  Serial.println(String() + "currentLayer:" + currentLayer);
//  Serial.println(String() + "falg run 1:" + flagS);
}

void doubleclick1(){
  Serial.println("button 2 klik run");
if(currentLayer == 0){
  currentLayer = 1;
  currentSelect = 1;
  cursorSelect();
  clearMenu();
}
  }

void getRotary(){
   newPosition = myEnc.read()/4;
  switch(currentLayer){
    case 1 :
     // if(currentLayer != lastLayer){ clearMenu();}
      lastLayer = currentLayer;     
      currentLength = menu1[0].toInt();
      //Serial.println(String()+"lastLayer2:"+lastLayer);
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
  

   if(currentLayer==1){
      Serial.println(String()+"currentSelect:"+currentSelect);
      if(currentSelect == 5 && stepLayer == 1){
        stepLayer=0; 
        if(currentSelect != lastStep){clearMenu();  }
        Serial.println(String()+"lastLayer:"+lastLayer);
        
        }
       if(currentSelect == 4 && stepLayer == 0){
        stepLayer=1; 
        if(currentSelect != lastStep){clearMenu();  }
        Serial.println(String()+"lastLayer:"+lastLayer);
        
        }
        lastStep = currentSelect; 
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

  if(subLayer==3){
     if(newPosition > oldPosition && timerLock < 10) {
     timerLock++;
     if(timerLock != lastLock){lcd.setCursor(9,2); lcd.print(panah[2]);}// }//lcd.setCursor(11,1);lcd.print(panah[panahRun]);} 
     }
     else if(newPosition < oldPosition && timerLock != 0){
      timerLock--;
      if(timerLock != lastLock){ lcd.setCursor(4,2); lcd.print(panah[1]); }//lcd.setCursor(11,1);lcd.print(panah[panahRun]);} 
     }
     // else if(timerLock == lastLock){lcd.setCursor(11,1); lcd.print(panah[0]);}
     delay(500);
    lastLock = timerLock;
     if(timerLock == lastLock){ lcd.setCursor(4,2);lcd.print(panah[0]); lcd.setCursor(9,2);lcd.print(panah[0]);}
  }
   
  if(subLayer==4){
     if(newPosition > oldPosition && timerSleep < 10) {
     timerSleep++;
     if(timerSleep != lastSleep){lcd.setCursor(9,2); lcd.print(panah[2]);}//panahRun = 2; }//lcd.setCursor(11,1);lcd.print(panah[panahRun]);} 
     }
     else if(newPosition < oldPosition && timerSleep != 0){
      timerSleep--;
      if(timerSleep != lastSleep){lcd.setCursor(4,2); lcd.print(panah[1]); }//lcd.setCursor(11,1);lcd.print(panah[panahRun]);} 
     }
     // else if(timerLock == lastLock){lcd.setCursor(11,1); lcd.print(panah[0]);}
     delay(500);
    lastSleep = timerSleep;
     if(timerSleep == lastSleep){ lcd.setCursor(4,2);lcd.print(panah[0]); lcd.setCursor(9,2);lcd.print(panah[0]);}
  }
 
  oldPosition = newPosition;
 
}


}

void showLcd(){
  int dataSensor[]={valueLength,valueWidth,valueHeight,0};
  if(currentLayer==0){
    lcd.setCursor(0,0);
    lcd.print("Panjang:");
    lcd.print((valueLength<10)?"0"+String(valueLength):valueLength);
    lcd.setCursor(13,0);
    lcd.print(" cm");
    
    lcd.setCursor(0,1);
    lcd.print("Lebar  :");
    lcd.print((valueWidth<10)?"0"+String(valueWidth):valueWidth);
    lcd.setCursor(13,1);
    lcd.print(" cm");
    
    lcd.setCursor(0,2);
    lcd.print("Tinggi :");
    lcd.print((valueHeight<10)?"0" + String(valueHeight):valueHeight);
    lcd.setCursor(13,2);
    lcd.print(" cm");

    lcd.setCursor(0,3);
    lcd.print("Berat  :");
    //lcd.setCursor(0,3);
    lcd.print(parWeight);
    //lcd.print((weight >= 1000)? weight / 1000 :  weight);
    lcd.setCursor(14,3);
    lcd.print("Gram");
    
   
  }

   if(currentLayer==1 ){
    //clearSelect();
    //lastStep = stepLayer;
    lcd.setCursor(18,cursorLayer );
    lcd.print("<-");

    if(currentSelect < 5){
      for(int i=0;i<4;i++){
      lcd.setCursor(0,i);
      lcd.print(menu1[i+1]);
      //Serial.println(String()+"menu1:"+menu1[i+1]);
    }
    }

    if(currentSelect > 4){
      for(int i=0;i<currentLength-4;i++){
      lcd.setCursor(0,i);
      lcd.print(menu1[5+i]);
      //Serial.println(String()+"menu1:"+menu1[4+i]);
      }
   // Serial.println(String()+"lastLayer2:"+lastLayer);
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
//      lcd.setCursor(10,(i==3)?2:i);
//      lcd.print(dataSensor[(i==3)?2:i]); 
      lcd.setCursor(13,(i==3)?2:i);
      lcd.print(" CM");
    }
    lcd.setCursor(10,0);
    lcd.print((valueLength<10)?"0"+String(valueLength):valueLength); 
    lcd.setCursor(10,1);
    lcd.print((valueWidth<10)?"0"+String(valueWidth):valueWidth); 
    lcd.setCursor(10,2);
    lcd.print((valueHeight<10)?"0" + String(valueHeight):valueHeight); 
    if(flagS){lcd.setCursor(12,cursorLayer); lcd.print("*");}
  }

  if(subLayer==3){
    lcd.setCursor(2,0);
    lcd.print("SET TIMER LOCK");
    lcd.setCursor(6,2);
    lcd.print((timerLock<10)?"0"+String(timerLock):timerLock);
    
   
    
//     Serial.println(String()+"timerLock    :"+timerLock);
//    Serial.println(String()+"lastLock     :"+lastLock);
    //if(panahRun == 0){ lcd.setCursor(11,1); lcd.print(panah[panahRun]);}
//    lcd.print(panah[panahRun]);//(state==1)?panah[panahRun]:panah[panahRun]);
    
  }

  if(subLayer==4){
    lcd.setCursor(2,0);
    lcd.print("SET TIMER SLEEP");
    lcd.setCursor(6,2);
    lcd.print((timerSleep<10)?"0"+String(timerSleep):timerSleep);
    
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
          case 5:
            cursorLayer = 0;
            break;
          case 6:
            cursorLayer = 1;
            break;
          case 7:
            cursorLayer = 2;
            break;
          case 8:
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
