#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DFRobot_HX711.h>

DFRobot_HX711 BERAT(A2, A3);

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const int trigPin1 = 9;  // Pin trigger sensor ultrasonik 1 (panjang)
const int echoPin1 = 8;//10; // Pin echo sensor ultrasonik 1 (panjang)

const int trigPin2 = 7;//11; // Pin trigger sensor ultrasonik 2 (lebar)
const int echoPin2 = 6;//12; // Pin echo sensor ultrasonik 2 (lebar)

const int trigPin3 = 13; // Pin trigger sensor ultrasonik 3 (tinggi)
const int echoPin3 = A0; // Pin echo sensor ultrasonik 3 (tinggi)

//const int loadCellPin = A1; // Pin load cell

const float referenceLength = 50.0; // Panjang referensi dalam cm
const float referenceWidth = 50.0;  // Lebar referensi dalam cm
const float referenceHeight = 50.0; // Tinggi referensi dalam cm
const float referenceWeight = 5.0; // Berat referensi dalam kg
const float maxLength = 50.0; // Batas Panjang maksimal dalam cm
const float maxWidth = 50.0; // Batas Lebar maksimal dalam cm
const float maxHeight = 50.0; // Batas Tinggi maksimal dalam cm
const float maxWeight = 5.0; // Batas berat maksimal dalam kg

int hasilP,hasilL=0;

void setup() {
  Serial.begin(9600); // Inisialisasi komunikasi serial
  lcd.init(); 
  lcd.backlight();
  lcd.setCursor(0,0);
 
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
}
long berat;
void loop() {
    kalkulasi();
    lcd.setCursor(0,0);
    lcd.print("hasil panjang:");
    lcd.print(hasilP);
    lcd.print("cm");
    lcd.setCursor(0,1);
    lcd.print("hasil lebar:");
    lcd.print(hasilL);
    lcd.print("cm");
    lcd.setCursor(0,2);
    lcd.print("hasil lebar:");
    lcd.print(hasilT);
    lcd.print("cm");
    lcd.setCursor(0,3);
    lcd.print("hasil berat:");
    //lcd.print(hasilL);
    //lcd.print("cm");
    
    showMonitor();

}

float getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2; // Rumus jarak dari waktu

  return distance;
}

void kalkulasi(){
  int panjang = getDistance(trigPin1, echoPin1);
  int lebar   = getDistance(trigPin2, echoPin2);  
  int tinggi  = getDistance(trigPin3, echoPin3);  
  
  if(panjang <= referenceLength){
    hasilP = referenceLength - panjang;
  }
  else{ hasilP = 0; }
  
  if(lebar <= referenceWidth){
    hasilL = referenceWidth - lebar;
  }
  else{ hasilL = 0; }
  
  if(tinngi <= heightDifference){
    hasilT = referenceWidth - lebar;
  }
  else{ hasilT = 0; }

  length = panjang;
  width  = lebar;
  height = tinggi;
}

void showMonitor(){
  Serial.print(" Panjang: ");
  Serial.println(length);
  Serial.println(" cm");

  Serial.print(" Lebar: ");
  Serial.println(width);
  Serial.println(" cm");

  Serial.print(" Tinggi: ");
  Serial.print(height);
  Serial.println(" cm");

  Serial.print("Berat: ");
  Serial.print(weight);
  Serial.println(" kg");
}
