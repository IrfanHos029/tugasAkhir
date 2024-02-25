#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
//#include <DFRobot_HX711.h>
#include <HCSR04.h>
#include "HX711.h"

HX711 scale(A2, A3); //HX711 scale(6, 5);

float calibration_factor = -9;
float units;
float ounces;

/*
 * pin Trigger ultrasonik : 5
 * pin Echo ultrasonik 1  : 6
 * pin Echo ultrasonik 2  : 7
 * pin Echo ultrasonik 3  : 8
 */
 
HCSR04 hc(5, new int[3]{6, 7, 8}, 3); //initialisation class HCSR04 (trig pin , echo pin, number of sensor)

//DFRobot_HX711 BERAT(A2, A3);

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

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
void setup() {
  Serial.begin(9600); // Inisialisasi komunikasi serial
  lcd.begin(); 
  scale.set_scale(calibration_factor);
  scale.tare();
  lcd.backlight();
  pinMode(buzzer, OUTPUT);
   

}
long berat;
void loop() {
   
    weight = getWeight();
    
    kalkulasi();
   
    
}

void showLCD(){
  
    lcd.setCursor(0,0);
    lcd.print("panjang:");
    lcd.print((hasilP >= 50)? 0 : hasilP);
    lcd.setCursor(13,0);
    lcd.print(" cm");
    
    lcd.setCursor(0,1);
    lcd.print("lebar  :");
    lcd.print((hasilL >= 50)? 0 : hasilL);
    lcd.setCursor(13,1);
    lcd.print(" cm");
    
    lcd.setCursor(0,2);
    lcd.print("tinggi :");
    lcd.print((hasilT >= 50)? 0 : hasilT);
    lcd.setCursor(13,2);
    lcd.print(" cm");
    
    lcd.setCursor(0,3);
    lcd.print("berat:");
    lcd.print((weight >= 1000)? weight / 1000 :  weight);
    lcd.setCursor(13,3);
    lcd.print((weight >= 1000)? "Gram" : "KG");
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
  static bool flag=false;
  
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
  else{ hasilP = hasilP; hasilL = hasilL; hasilT = hasilT; trigger = false; runObject = false; timeRead=0; flag=1;}
 
 }
 while(weight >= 1000){lcd.clear(); break;}
 while(weight < 1000){lcd.clear(); break;}
 showLCD();
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


  Serial.print("Berat: ");
  Serial.print((weight >= 1000)? weight / 1000 : weight);
  Serial.println(" KG");
  
  Serial.print("Berat: ");
  Serial.print(weight);
  Serial.println(" gram");

//  Serial.print("trigger: ");
//  Serial.println(trigger);
//
//  Serial.print("runObject: ");
//  Serial.println(runObject);
  
}

float getWeight(){
 
  unsigned long tmr = millis();
  static unsigned long saveTmr=0;

  if(tmr - saveTmr > 1000 && runObject == true){
  saveTmr = tmr;
// Serial.println("run ");
  units = scale.get_units(),3;
  if (units < 0)
  {
    units = 0.00;
  }
  if(units > 0){
    trigger = true;
  }
  
 // else{ runObject = 0; }
 // ounces = units * 0.035274;
  Serial.print("run ");
  return units;
  }
}

void buzzerRun(int flag){
  if(flag){digitalWrite(buzzer,HIGH); }
  else{ digitalWrite(buzzer,LOW); }
}
