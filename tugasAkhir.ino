#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <HCSR04.h>
#include "HX711.h"

HX711 scale(11, 10); 

float calibration_factor = -9; //rubah nilai ini sesuai hasil dari nilai kalibrasi

float ounces;

/*
 * pin Trigger ultrasonik : 5
 * pin Echo ultrasonik 1  : 6
 * pin Echo ultrasonik 2  : 7
 * pin Echo ultrasonik 3  : 8
 */
 
HCSR04 hc(5, new int[3]{6, 7, 8}, 3); //initialisation class HCSR04 (trig pin , echo pin, number of sensor)

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display

#define buzzer 10


const int referenceLength = 50; // Panjang referensi dalam cm
const int referenceWidth = 50;  // Lebar referensi dalam cm
const int referenceHeight = 50; // Tinggi referensi dalam cm
const float referenceWeight = 5.0; // Berat referensi dalam kg
const float maxLength = 50.0; // Batas Panjang maksimal dalam cm
const float maxWidth = 50.0; // Batas Lebar maksimal dalam cm
const float maxHeight = 50.0; // Batas Tinggi maksimal dalam cm
const float maxWeight = 5.0; // Batas berat maksimal dalam kg

int hasilP,hasilL,hasilT;
int length,height,width;
float weight;
bool runObject = true;
bool trigger   =false;
char *textWeight[]{" Gram"," KG  "};
 bool stateTimerLCD=true;

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
int flagTr=1;
 int timerFlag;
 int flagFsh=0;;
 int stateRnn=1;
 float units=-1;
void setup() {
  Serial.begin(9600); // Inisialisasi komunikasi serial
  lcd.begin(); 
  scale.set_scale(calibration_factor);
  scale.tare();
  lcd.backlight();
  pinMode(buzzer, OUTPUT);
  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);
  lcd.createChar(6, speaker);

//  for(int i = 0; i < 100; i++){
//    lcd.setCursor(0,0);
//    lcd.print("LOADING..");
//    lcd.setCursor(16,0);
//    lcd.print(i);
//    lcd.print("%");
//    updateProgressBar(i, 100, 1);
//    delay(50);
//  }
  lcd.clear();
  Serial.println("build run");
}

void loop() {
   
    weight = getWeight();
    
    kalkulasi();
   showLCD();
   
   if(trigger == false && flagTr == 1 && flagFsh == 0){
    Serial.print("timerFlag:");
    Serial.println(timerFlag);
    while(timerFlag>=20){
      timerLCD(0,1);
      break;
    }
  }
  
   
//  else{
//    
//    timerLCD(0);
//  }
//      Serial.print("trigger: ");
//  Serial.println(trigger);
//    Serial.print("falgTr: ");
//  Serial.println(flagTr);
}

void updateProgressBar(unsigned long count, unsigned long totalCount, int lineToPrintOn)
 {
    double factor = totalCount/100.0;          //See note above!
    int percent = (count+1)/factor;
    int number = percent/5;
    int remainder = percent%5;
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
void showLCD(){
    
    if(flagTr==true){
    lcd.setCursor(0,0);
    lcd.print("Panjang:");
    lcd.print((hasilP >= 50)? 0 : hasilP);
    lcd.setCursor(13,0);
    lcd.print(" cm");
    
    lcd.setCursor(0,1);
    lcd.print("Lebar  :");
    lcd.print((hasilL >= 50)? 0 : hasilL);
    lcd.setCursor(13,1);
    lcd.print(" cm");
    
    lcd.setCursor(0,2);
    lcd.print("Tinggi :");
    lcd.print((hasilT >= 50)? 0 : hasilT);
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
    }
    else{
        lcd.setCursor(18,3);
        lcd.print(" ");
        lcd.setCursor(19,3);
        lcd.print(" ");
    }
    }
    else{lcd.clear();}
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
  // check RunSelector
  //if(!dwDo(Run)) return;
  unsigned long tmr = millis();
  static unsigned long saveTmr1=0;
  static unsigned long saveTmr2=0;
  static byte timeRead,x=0;
  static int panjang,lebar,tinggi;
 // static bool flag=false;
  //static byte flagWeight;
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
  
 if(tmr - saveTmr2 > 50 && trigger == true){
  saveTmr2 = tmr;
  
 
  if(timeRead <= 100){
    timeRead++;
    if(panjang <= referenceLength){
    hasilP = referenceLength - panjang;
  }
  
  
  if(lebar <= referenceWidth){
    hasilL = referenceWidth - lebar;
  }
  
  
  if(tinggi <= referenceHeight){
    hasilT = referenceHeight - tinggi;
  }
 
  showMonitor();
  }
  else{ hasilP = hasilP; hasilL = hasilL; hasilT = hasilT; trigger = false; runObject = false; timeRead=0; flagFsh=1;}
 
 }
 
// if(weight >= 1000){flagWeight=1;}
// else{flagWeight=0;}
// 
 
  length = panjang;
  width  = lebar;
  height = tinggi;
}
void showMonitor(){
//  Serial.print("Panjang: ");
//  Serial.print(length);
//  Serial.println(" cm");
//
//  Serial.print("Lebar: ");
//  Serial.print(width);
//  Serial.println(" cm");
//
//  Serial.print("Tinggi: ");
//  Serial.print(height);
//  Serial.println(" cm");


//  Serial.print("Berat: ");
//  Serial.print((weight >= 1000)? weight / 1000 : weight);
//  Serial.println(" KG");
//  
//  Serial.print("Berat: ");
//  Serial.print(weight);
//  Serial.println(" gram");

  Serial.print("trigger: ");
  Serial.println(trigger);

  Serial.print("runObject: ");
  Serial.println(runObject);
  
}

float getWeight(){
 
  unsigned long tmr = millis();
  static unsigned long saveTmr=0;
 static int flagR=0;
 
  if(tmr - saveTmr > 1000 && runObject == true  ){
  saveTmr = tmr;
// Serial.println("run ");
  units = scale.get_units(),3;
  if (units < 0 )
  {
    units = 0.00; 
  }
  if(units <= 0 && stateRnn == 1){ timerLCD(1,1); }
 // ounces = units * 0.035274;
 // Serial.println("run ");
 if(units > 1000){
    trigger = true;
    flagTr=1;;
    timerLCD(0,0);
  }
  return units;
  }
  
  
}
 
//void timerLCD(i){
//  unsigned long tmr = millis();
//  static unsigned long saveTmr;
//  static int timerFlag;
//  //static  int flag1=1;
//  if(tmr - saveTmr > 50 && flagTr == true && trigger != true){
//    saveTmr = tmr;
//    
//    Serial.println("tmr run ");
//    if(timerFlag <= 100){  timerFlag++; stateTimerLCD = true;Serial.println(String() + "stateTimerLCD:" + stateTimerLCD); }
//    else{ timerFlag=0; stateTimerLCD=false; flagTr == false; Serial.println(String() + "stateTimerLCD:" + stateTimerLCD);}
//  }
//  
//  
//}
void timerLCD(int state,int runn){
  unsigned long tmr = millis();
  static unsigned long saveTmr;
  //static int timerFlag;
  //static  int flag1=1;
  if(tmr - saveTmr > 50 && state == 1 && runn){
    saveTmr = tmr;
    timerFlag++;
    flagTr=1;
    stateRnn = 1;
    Serial.println("tmr run ");
  }
  else{
    saveTmr = 0;
    timerFlag=0;
    if(runn){flagTr=0;}
    stateRnn = 0;
    //timerLCD(0);
  }

  
  
}



void buzzerRun(int flag){
  if(flag){digitalWrite(buzzer,HIGH); }
  else{ digitalWrite(buzzer,LOW); }
}
