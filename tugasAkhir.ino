/*
 * pin Trigger ultrasonik : 5
 * pin Echo ultrasonik 1  : 2
 * pin Echo ultrasonik 2  : 3
 * pin Echo ultrasonik 3  : 4
 * 
 * led[0] = indikator run
 * led[1] = indikator lock
 * led[2] = indikator kalibrasi
 */
 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <HCSR04.h>
#include "HX711.h"
#include <OneButton.h>
#include <Encoder.h>
#include <EEPROM.h>

#define encoderDTpin 2
#define encoderCLKpin 3
#define loadCelDTpin 6
#define loadCellSCKpin 7
#define triggerPin 5
#define echoPin1   9//2
#define echoPin2   10//3
#define echoPin3   4
#define buttonReset 8
#define buzzer 11

int led[]={11,12,13};

HX711 scale(loadCelDTpin, loadCellSCKpin); 
Encoder myEnc(encoderDTpin, encoderCLKpin);
OneButton button0(buttonReset, true);
HCSR04 hc(triggerPin, new int[3]{echoPin1, echoPin2, echoPin3}, 3); //initialisation class HCSR04 (trig pin , echo pin, number of sensor)
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display




byte cursorLayer = 0;
byte subLayer=0;
byte currentLayer =0;
bool flagTr  = true;
bool stepLayer=1;
bool flagS=0;
bool stateRun= 1;
bool runObject = true;
bool trigger   = false;
bool state1 = 0;
char hasilP,hasilL,hasilT;
int  length,height,width;
int  timerFlag;
int  timeRead = 0;
int lastStep,lastSleep,lastLock;
int timerLock,timerSleep;
int valueLength = 0;
int valueWidth = 0;
int valueHeight = 0;
int currentSelect = 1;
int currentLength;
float units   = -1;
float unitSetting = -1;
float ounces;
float parWeight=900;
float weight;
long oldPosition  = 0;
long newPosition = 0;
unsigned long saveTmrH;

int referenceLength = 0; // Panjang referensi dalam cm
int referenceWidth  = 0;  // Lebar referensi dalam cm
int referenceHeight = 0; // Tinggi referensi dalam cm

float calibration_factor = -9; //rubah nilai ini sesuai hasil dari nilai kalibrasi

String menu1[]={"5","kalibrasi Beban","Kalibrasi Dimensi","Set Timer Lock","Set Timer Sleep","Back"};
String menuJarak[]={"4","Sensor 1:","Sensor 2:","Sensor 3:","Back"};
char *panah[]{" ","<",">"};
char *textWeight[]{" Gram"," KG  "};

byte zero[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte one[] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};

byte two[] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
};

byte three[] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
};

byte four[] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};

byte five[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte speaker[] = {
  B00010,
  B00110,
  B11110,
  B11110,
  B11110,
  B11110,
  B00110,
  B00010
};

byte pointer[] = {
  B00000,
  B00100,
  B01000,
  B11111,
  B01000,
  B00100,
  B00000,
  B00000
};

void setup() {
  Serial.begin(9600); // Inisialisasi komunikasi serial
  //lcd.init(); 
  lcd.begin();
  lcd.backlight();
  pinMode(buzzer, OUTPUT);
  for(int i = 0; i < 3; i++){pinMode(led[i],OUTPUT);}
  button0.attachClick(singleClick);
  button0.attachDoubleClick(doubleclick1);
  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);
  lcd.createChar(6, speaker);
  lcd.createChar(7, pointer);

  parWeight = EEPROM.read(0);
  valueLength = EEPROM.read(1);
  valueWidth = EEPROM.read(2);
  valueHeight = EEPROM.read(3);
  timerLock =  EEPROM.read(4);
  timerSleep =EEPROM.read(5);
  referenceLength= valueLength;
  referenceWidth = valueWidth;
  referenceHeight= valueHeight;
  calibration_factor = parWeight;
  scale.set_scale(calibration_factor);
  scale.tare();
//  for(int i = 0; i < 100; i++){
//    lcd.setCursor(0,0);
//    lcd.print("LOADING..");
//    lcd.setCursor(16,0);
//    lcd.print(i);
//    lcd.print("%");
//    updateProgressBar(i, 100, 1);
//    delay(50);
//  }
  Serial.println(String()+"referenceLength:"+referenceLength);
  Serial.println(String()+"referenceWidth :"+referenceWidth);
  Serial.println(String()+"referenceHeight:"+referenceHeight);
  Serial.println(String()+"calibration_factor:"+calibration_factor);
  lcd.clear();
  // Serial.println(String()+"timerSleep:"+timerSleep);
  // Serial.println("build run");
  // Serial.println(String() + "trigger  :" + trigger);
  // Serial.println(String() + "runObject:" + runObject);
  // Serial.println(String() + "timeRead :" + timeRead);
  // Serial.println(String() + "stateRun :" + stateRun);
}

void loop() {
    button0.tick();
    getRotary();
    kalkulasi();
    weight = getWeight();
    timerLCD(); 
    showSetting();
    showLed();
    //Serial.println(String()+"state1:"+ state1);
}

void showLed(){
    if(state1 == 1 && runObject == 1 && currentLayer == 0){getIndikator(0,1,0);}
    else if(state1 == 0 && runObject == 1 && currentLayer == 0){getIndikator(0,0,1); getIndikator(1,0,0); getIndikator(2,0,0);}
    else if(state1 == 0 && runObject == 0 && currentLayer == 0){getIndikator(0,0,0); getIndikator(1,0,1); getIndikator(2,0,0);}
    else if(currentLayer == 1 && subLayer == 0){getIndikator(0,0,0); getIndikator(1,0,0); getIndikator(2,0,1);}
    else if(currentLayer != 1 && subLayer == 1){ getIndikator(2,1,0); }
    else if(currentLayer != 1 && subLayer == 2 && flagS == 0){ getIndikator(2,0,1); }
    else if(currentLayer != 1 && subLayer == 2 && flagS == 1){ getIndikator(2,1,0); }
    else if(currentLayer != 1 && subLayer == 3){ getIndikator(2,1,0); }
    else if(currentLayer != 1 && subLayer == 4){ getIndikator(2,1,0); }
}


void singleClick(){
 
  Serial.println("button 1 klik run");
  
 if(currentLayer == 0 && subLayer == 0 ){ 
    
    lcd.clear();
    hasilP  = 0; 
    hasilL  = 0; 
    hasilT  = 0; 
    weight  = 0.00;
    trigger = false; 
    runObject = true; 
    timeRead  = 0; 
    state1=0;
    stateRun  = 0;
    Serial.println(String() + "trigger  :" + trigger);
    Serial.println(String() + "runObject:" + runObject);
    Serial.println(String() + "timeRead :" + timeRead);
    Serial.println(String() + "stateRun :" + stateRun);
  }

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
      timeRead  = 0; 
      timerFlag = 0;
      state1=0;
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
 }

 else if(currentLayer != 1 && subLayer == 3 ){
   clearMenu();
   subLayer = 0;
   currentLayer = 1;
   currentSelect = 3;
   cursorSelect();
   EEPROM.write(4,timerLock);
 }

 else if(currentLayer != 1 && subLayer == 4 ){
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
      referenceLength= valueLength;
      referenceWidth = valueWidth;
      referenceHeight= valueHeight;
    break;
  };
 }
}

void doubleclick1(){
  Serial.println("button 2 klik run");
 if(currentLayer == 0){
    currentLayer = 1;
    currentSelect = 1;
    cursorSelect();
    clearMenu();
    trigger = false; 
    stateRun  = 0;
    state1 = 0;
 }
}

void getRotary(){
  newPosition = myEnc.read()/4;
 switch(currentLayer){
    case 1 :
      flagTr  = 1;
      state1 = 0;
      currentLength = menu1[0].toInt();
    break;
 };

 switch(subLayer){
    case 1 :
      currentLength = 1;
    break;
     
    case 2 :
      currentLength = menuJarak[0].toInt();
    break;

    case 3 :
      currentLength = 1;
    break;
 };

 if (newPosition != oldPosition){
  if (newPosition < oldPosition && currentSelect < currentLength && flagS == 0) {
    clearSelect();
    currentSelect++;
    cursorSelect(); 
  }

  else if(newPosition > oldPosition && currentSelect != 1 && flagS == 0){
    clearSelect();
    currentSelect--;
    cursorSelect();
  }
  
  if(currentLayer==1){
    
    if(currentSelect == 5 && stepLayer == 1){
      stepLayer=0; 
      if(currentSelect != lastStep){clearMenu();  }
    }

    if(currentSelect == 4 && stepLayer == 0){
      stepLayer=1; 
      if(currentSelect != lastStep){clearMenu();  }
    }
      lastStep = currentSelect; 
  }
    
  if(subLayer==1){
    if(newPosition < oldPosition && parWeight < 1000) { parWeight++; }
    else if(newPosition > oldPosition && parWeight != -50){ parWeight--; }
  }

  if(subLayer==2 && currentSelect == 1 && flagS == 1){
    if(newPosition < oldPosition && valueLength < 50) { valueLength++; } 
    else if(newPosition > oldPosition && valueLength > 0){ valueLength--; }  
  }

  if(subLayer==2 && currentSelect == 2 && flagS == 1){
    if(newPosition < oldPosition && valueWidth < 50) {  valueWidth++; }
    else if(newPosition > oldPosition && valueWidth > 0){ valueWidth--; }
  }

  if(subLayer==2 && currentSelect == 3 && flagS == 1){
    if(newPosition < oldPosition && valueHeight < 50) { valueHeight++; }
    else if(newPosition > oldPosition && valueHeight > 0){ valueHeight--; }
  }

  if(subLayer==3){
    if(newPosition < oldPosition && timerLock < 20) {
      timerLock++;
      if(timerLock != lastLock){ lcd.setCursor(9,2); lcd.print(panah[2]);}
    }

    else if(newPosition > oldPosition && timerLock != 0){
      timerLock--;
      if(timerLock != lastLock){ lcd.setCursor(4,2); lcd.print(panah[1]); }
    }

    delay(500);
    lastLock = timerLock;
    if(timerLock == lastLock){lcd.setCursor(4,2);lcd.print(panah[0]); lcd.setCursor(9,2);lcd.print(panah[0]);}
  }
   
  if(subLayer==4){
    if(newPosition < oldPosition && timerSleep < 20) {
      timerSleep++;
      if(timerSleep != lastSleep){lcd.setCursor(9,2); lcd.print(panah[2]);}
    }

    else if(newPosition > oldPosition && timerSleep != 0){
      timerSleep--;
      if(timerSleep != lastSleep){lcd.setCursor(4,2); lcd.print(panah[1]); }
    }

    delay(500);
    lastSleep = timerSleep;
    if(timerSleep == lastSleep){ lcd.setCursor(4,2);lcd.print(panah[0]); lcd.setCursor(9,2);lcd.print(panah[0]);}
  }
  oldPosition = newPosition;
 }
}


void updateProgressBar(unsigned long count, unsigned long totalCount, int lineToPrintOn)
 {
    double factor    = totalCount/100.0;          //See note above!
    int    percent   = (count+1)/factor;
    int    number    = percent/5;
    int    remainder = percent%5;
    if(number > 0)
    {
      for(int j = 0; j < number; j++)
      {
       lcd.setCursor(j,lineToPrintOn);
       lcd.write(5);
      }
    }
       lcd.setCursor(number,lineToPrintOn);
       lcd.write(remainder); 
     if(number < 20)
    {
      for(int j = number+1; j <= 20; j++)
      {
        lcd.setCursor(j,lineToPrintOn);
       lcd.write(0);
      }
    }  
 }


void showSetting(){
  int dataSensor[]={valueLength,valueWidth,valueHeight,0};
  if(currentLayer==0 && subLayer == 0){

   if(flagTr==true){
  
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Panjang:");
    lcd.print((hasilP>=referenceLength)?"00" :(hasilP<10)?"0"+String(hasilP):hasilP);
    lcd.setCursor(13,0);
    lcd.print(" cm");
    
    lcd.setCursor(0,1);
    lcd.print("Lebar  :");
    lcd.print((hasilL>=referenceWidth)?"00" :(hasilL<10)?"0"+String(hasilL):hasilL);
    lcd.setCursor(13,1);
    lcd.print(" cm");
    
    lcd.setCursor(0,2);
    lcd.print("Tinggi :");
    lcd.print((hasilT>=referenceHeight)?"00" :(hasilT<10)?"0"+String(hasilT):hasilT);
    lcd.setCursor(13,2);
    lcd.print(" cm");

    lcd.setCursor(0,3);
    lcd.print("Berat  :");
    lcd.print((weight >= 1000)? weight / 1000 :  weight);
    lcd.setCursor(13,3);
    lcd.print((weight >= 1000)?textWeight[1]:textWeight[0]);
    if(weight >= 5000){
        lcd.setCursor(18,3);
        lcd.print("!");
        lcd.setCursor(19,3);
        lcd.write(6);
        if(runObject)buzzerRun(1); 
    }
    else{
        lcd.setCursor(18,3);
        lcd.print(" ");
        lcd.setCursor(19,3);
        lcd.print(" ");
        buzzerRun(0);
    }
   }
   else{ lcd.noBacklight(); clearMenu();}
  }

  if(currentLayer==1 && subLayer == 0){
    lcd.backlight();
    lcd.setCursor(18,cursorLayer );
    lcd.write(byte(7));
    //Serial.println(String() + "stateRun :"+stateRun);
    //Serial.println(String() + "currentSelect:"+currentSelect);
    if(currentSelect < 5){
      for(int i=0;i<4;i++){
        lcd.setCursor(0,i);
        lcd.print(menu1[i+1]);
        //Serial.println(String() + "menu1[i+1]:"+menu1[i+1]);
      }

      for(int i=0;i<2;i++){
        lcd.setCursor(19,i);
        lcd.print("|");
      }
    }

    if(currentSelect > 4){
      for(int i=0;i<currentLength-4;i++){
        lcd.setCursor(0,i);
        lcd.print(menu1[5+i]);
        //Serial.println(String() + "menu1[5+i]:"+menu1[5+i]);
      }

      for(int i=0;i<2;i++){
        lcd.setCursor(19,2+i);
        lcd.print("|");
      }
    }
  }

   
  if(subLayer==1){
    scale.set_scale(calibration_factor);
    lcd.setCursor(0,0);
    lcd.print("BERAT:");
    lcd.setCursor(6,1);
    lcd.print((getWeightSet() >= 1000)? getWeightSet() / 1000 :  getWeightSet());
    lcd.setCursor(11,1);
    lcd.print((getWeightSet() >= 1000)?textWeight[1]:textWeight[0]);
    lcd.setCursor(0,3);
    lcd.print("Parameter : ");
    lcd.print(parWeight);
    Serial.println(String()+"getWeightSet():"+getWeightSet());
  }

  if(subLayer==2){
    lcd.setCursor(17,cursorLayer ); 
    lcd.write(byte(7));
     
    for(int i=0;i<currentLength;i++){
      lcd.setCursor(0,i);
      lcd.print(menuJarak[i+1]);  
      lcd.setCursor(13,(i==3)?2:i);
      lcd.print("CM");
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
  }

  if(subLayer==4){
    lcd.setCursor(2,0);
    lcd.print("SET TIMER SLEEP");
    lcd.setCursor(6,2);
    lcd.print((timerSleep < 1)?"OFF" :(timerSleep<10)?"0"+String(timerSleep)+" ":timerSleep);
  }
}

/*
void kalkulasi(){
  int panjang = 0;//hc.dist(0);
  //delay(60);
  int lebar   = 0;//hc.dist(1); 
  delay(60);
  int tinggi  = hc.dist(2);
  delay(60); 
  
  if(panjang <= referenceLength){
    hasilP = referenceLength - panjang;
  }
  else{ hasilP = 0; }
  
  if(lebar <= referenceWidth){
    hasilL = referenceWidth - lebar;
  }
  else{ hasilL = 0; }
  
  if(tinggi <= referenceHeight){
    hasilT = 50 - tinggi;
  }
  //else{ hasilT = 0; }
  lcd.setCursor(0,2);
    lcd.print("tinggi:");
    lcd.print((hasilT >= 50)? 0 : hasilT);
     lcd.setCursor(13,2);
    lcd.print(" cm");

  length = panjang;
  width  = lebar;
  height = tinggi;
}*/

void kalkulasi(){
  
  unsigned long        tmr = millis();
  static unsigned long saveTmr1 = 0;
  static unsigned long saveTmr2 = 0;
  static int           x = 0;
  static int           panjang,lebar,tinggi;
 
  if(tmr - saveTmr1 > 60 && trigger == true){
    saveTmr1 = tmr;
  
    switch(x){
      case 0 :
        panjang = hc.dist(0);
        break;
      case 1 :
        lebar   = hc.dist(1);
        break;
      case 2 :
        tinggi  = hc.dist(2);
        break;
      case 3 :
        x = 0;
        break;
    };
    x++;
  }
  
  if(tmr - saveTmr2 > 1000 && trigger == true){
    saveTmr2 = tmr;
  
   
    if(timeRead <= timerLock){
      timeRead++;
      if(panjang <= referenceLength){ hasilP = referenceLength - panjang; }
    
      if(lebar <= referenceWidth){ hasilL = referenceWidth - lebar; }
      
      if(tinggi <= referenceHeight){ hasilT = referenceHeight - tinggi; }
     
      //showMonitor();
    }
    else{
      lcd.noBacklight();
      buzzerRun(1); 
      delay(50); 
      lcd.backlight(); 
      buzzerRun(0); 
      hasilP = hasilP;
      hasilL = hasilL;
      hasilT = hasilT; 
      //Serial.print(String()+"hasilL:" + hasilL); 
      trigger = false; 
      runObject = false; 
      timeRead=0; 
      stateRun = 0;
    }
  
  }
  
  length = panjang;
  width  = lebar;
  height = tinggi;
}

void showMonitor(){
  Serial.print("Panjang: ");
  Serial.print(length);
  Serial.println(" cm");
 
  Serial.print("Lebar: ");
  Serial.print(width);
  Serial.println(" cm");
 
  Serial.print("Tinggi: ");
  Serial.print(height);
  Serial.println(" cm");
 
  Serial.print("Berat: ");
  Serial.print((weight >= 1000)? weight / 1000 : weight);
  Serial.println(" KG");
  
  Serial.print("Berat: ");
  Serial.print(weight);
  Serial.println(" gram");

  Serial.print("trigger: ");
  Serial.println(trigger);

  Serial.print("runObject: ");
  Serial.println(runObject);
  
}

float getWeight(){
 
  unsigned long tmr = millis();
  static unsigned long saveTmr=0;
  static int objek = 1000;//
 
  if(tmr - saveTmr > 1000 && runObject == true  && currentLayer == 0){
    saveTmr = tmr;
    units = scale.get_units(),3;
  
    if (units < 0 ){ units = 0.00; }
    if(units < objek && trigger == false){ stateRun = 1; }
  
    // ounces = units * 0.035274;
   
    if(units > objek ){
      trigger = true;
      flagTr  = 1;
      stateRun = 0;
      state1=0;
      clearChar(18,0);
      clearChar(19,0);
    }
    return units;
  }
}

float getWeightSet(){
  unsigned long tmr = millis();
  static unsigned long saveTmr=0;

  if(tmr - saveTmr > 1000){
    //scale.power_up();
    saveTmr = tmr;
    unitSetting = scale.get_units(),3;
    if (unitSetting < 0 ){ unitSetting = 0.00; }
    return unitSetting;
  }
}

void timerLCD(){
  static int Run;
  int Delay = timerSleep*1000;
  static int co;
  
  if(timerSleep > 0){ Run = 1; } else{ Run = 0; }
  //Serial.println(String() + "stateRun :"+stateRun);
  if(stateRun == 1 && Run == 1){
    flagTr=1;
    unsigned long tmrH = millis();
    lcd.setCursor(18,0);
    lcd.print(co);
    if(state1==0){co = (millis() - saveTmrH)/1000;}
  }

  else{ saveTmrH = millis(); co = 0;  }
  
  if((millis() - saveTmrH) > Delay && stateRun == 1 && Run == 1)
  {
    //Serial.println("tmr run ");
    if(state1==0 && stateRun == 1){state1 = 1;}
  }
  if(state1 == 1){ flagTr=0; clearChar(18,0); clearChar(19,0); }
}

void buzzerRun(bool flag){
  if(flag){digitalWrite(buzzer,HIGH); }
  else{ digitalWrite(buzzer,LOW); }
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
  lcd.setCursor (17,0);
  lcd.print("  ");
  lcd.setCursor (17,1);
  lcd.print("  ");
  lcd.setCursor (17,2);
  lcd.print("  ");
  lcd.setCursor (17,3);
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


void getIndikator(int conLed,int stateRun,int stateLed){

  unsigned long tmr = millis();
  static unsigned long saveTmr;
  static bool state=false;

  if((tmr - saveTmr) > 1000 && stateRun == 1){
    state = !state;
    digitalWrite(led[conLed],state);
    saveTmr = tmr;
  }
 
  else if(stateRun == 0){ digitalWrite(led[conLed],stateLed); }
  
}
