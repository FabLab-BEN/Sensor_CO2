//---CAPTEUR DE CO2----Novembre 2020 - Tony Vanpoucke (Edulab, Rennes 2)----------------------------//
// ----- PARAMETRES MODIFIABLES ----------------------------------//



int SensorInterval = 2000;  //Interval de capture du CO2

float luminosite = 3;     //luminosité des écrans
int loadTime = 10000;     //temps de "chauffe" du capteur CO2
int loadSpeed = 40;       //rapiditée de l'animation de chargement
int messageInterval = 4;  //fréquence des messages d'alerte si un seuil CO2 est passé (4 par défaut)
unsigned long getDatatimer;

// ----- Variables du programme

int taux_co2;
int messageBox = 0;

// ----- Bibliothèques utilisées
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"
#include <SoftwareSerial.h>
#include <TM1637Display.h>
SCD30 airSensor;

#define BAUDRATE 9600                       // fréquence de fonctionnement du capteur CO2 (ne pas changer)

// ----- Branchements ESP8266

// Branchement capteur SCD30
#define RX_PIN 5
#define TX_PIN 4

// Branchement écran 7 segement
#define CLK 14
#define DIO 12

// Raccordement électronique au ESP8266

SoftwareSerial mySerial(RX_PIN, TX_PIN);    
TM1637Display displayScreen(CLK, DIO);
unsigned long getDataTimer;


int ledV = 16; // La première led est reliée à la sortie 14 qui sur le microcontroleur correspond au D5
int ledO = 0; // La deuxième led est reliée à la sortie 12 qui sur le microcontroleur correspond au D6
int ledR = 13; // La troisième led est reliée à la sortie 13 qui sur le microcontroleur correspond au D7
int ledB = 15; // La quatrième led est reliée à la sortie 15 qui sur le microcontroleur correspond au D8

void setup() {
  // put your setup code here, to run once:
  
Serial.begin(9600); // Ouvre le port série à 9600 bauds
Serial.println("SCD30 Example");
Wire.begin();
if (airSensor.begin() == false)
{
Serial.println("Air sensor not detected. Please check wiring. Freezing...");
while (1);
}
pinMode(ledR, OUTPUT); // Broche 13 en sortie
pinMode(ledO, OUTPUT); // Broche 12 en sortie
pinMode(ledV, OUTPUT); // Broche 14 en sortie
pinMode(ledB, OUTPUT); // Broche 15 en sortie

 // Initialise la lecture du capteur C02
  airSensor.begin(BAUDRATE);
  airSensor.begin(mySerial);
  airSensor.setAutoSelfCalibration(true);
  airSensor.setMeasurementInterval(2);
 

  taux_co2 = airSensor.getCO2();            // Capture de la donnée pour "chauffer" le capteur de CO2

  displayScreen.setBrightness(luminosite); // Applique la lumisosité à l'ecran 7 seg

  loadingAnimation();                    // Lance une animation écran le temps de "chauffer" la capteur CO2

}

// ----- PROGRAMME ---------------------------------------//

void loop() 
{
  // put your main code here, to run repeatedly:
while (airSensor.dataAvailable())
{
taux_co2 = (int)airSensor.getCO2();
Serial.print("co2(ppm):");
Serial.print(taux_co2);
Serial.print(" temp(C):");
Serial.print(airSensor.getTemperature(), 1);
Serial.print(" humidity(%):");
Serial.print(airSensor.getHumidity(), 1);
Serial.println();
if (taux_co2 <= 600){ // Si taux_co2 inférieur ou égal à 600 alors
digitalWrite(ledB,HIGH); // La led bleu s'allume 
digitalWrite(ledV,LOW); // La led verte reste éteinte
digitalWrite(ledO,LOW); // La led orange reste éteinte
digitalWrite(ledR,LOW); // La led rouge reste éteinte
}
else if (taux_co2 <= 800){ // Si sinon le taux_co2 inférieur ou égal à 800 alors
digitalWrite(ledB,LOW); // La led bleu reste éteinte
digitalWrite(ledV,HIGH); // La led verte s'allume
digitalWrite(ledO,LOW); // La led orange reste éteinte
digitalWrite(ledR,LOW); // La led rouge reste éteinte
}
else if (taux_co2 >= 800 && taux_co2 <= 1000){ // Si sinon taux_co2 supérieur ou égal à 800 et inférieur ou égale à 1000 alors
digitalWrite(ledB,LOW); // La led bleu reste éteinte
digitalWrite(ledV,LOW); // La led verte reste éteinte
digitalWrite(ledO,HIGH); // La led orange s'allume
digitalWrite(ledR,LOW); // La led rouge reste éteinte
}
else if (taux_co2 >= 1000){ // Si sinon taux_co2 supérieur ou égal à 1000 alors
digitalWrite(ledB,LOW); // La led bleu reste éteinte
digitalWrite(ledV,LOW); // La led verte reste éteinte
digitalWrite(ledO,LOW); // La led orange reste éteinte
digitalWrite(ledR,HIGH); // La led rouge s'allume
}
}
delay(500); // Pause de 5 secondes

 // Par intervals régulier capture la donnée et l'affiche :
  if (millis() - getDataTimer >= SensorInterval)
  {
    
    taux_co2 = airSensor.getCO2();    // Numérisation de la donnée de CO2
    
    Serial.println(taux_co2);    // affichage du taux de CO2 sur le port ESP8266
    
    getDataTimer = millis();   // stocker les donnée d'interval

    screenDisplay();           // afficher la donnee sur l'ecran 7 segement (si un seuil est atteint, declanche animation)
    
  }
  
}


// --------------------------------------------------------//

void loadingAnimation()
{
  uint8_t loadA[] = { 0x30, 0x00, 0x00, 0x00 };                //animation de chargment 1/5
  uint8_t loadB[] = { 0x06, 0x30, 0x00, 0x00 };                //animation de chargment 2/5
  uint8_t loadC[] = { 0x00, 0x06, 0x30, 0x00 };                //animation de chargment 3/5
  uint8_t loadD[] = { 0x00, 0x00, 0x06, 0x30 };                //animation de chargment 4/5
  uint8_t loadE[] = { 0x00, 0x00, 0x00, 0x06 };                //animation de chargment 5/5

  //Affiche l'animation en boucle
  for (int i = 0; i <= loadSpeed; i++) {
    displayScreen.setSegments(loadA);
    delay((loadTime / loadSpeed) / 5);
    displayScreen.setSegments(loadB);
    delay((loadTime / loadSpeed) / 5);
    displayScreen.setSegments(loadC);
    delay((loadTime / loadSpeed) / 5);
    displayScreen.setSegments(loadD);
    delay((loadTime / loadSpeed) / 5);
    displayScreen.setSegments(loadE);
    delay((loadTime / loadSpeed) / 5);
   
  }
}


void screenDisplay()
{
  
   if (millis() - getDataTimer >= SensorInterval)
  {
    
    taux_co2 = airSensor.getCO2();    // Numérisation de la donnée de CO2
    
    Serial.println(taux_co2);    // affichage du taux de CO2 sur le port ESP8266
    
    getDataTimer = millis();   // stocker les donnée d'interval

    screenDisplay();           // afficher la donnee sur l'ecran 7 segement 
    
  }
  
  if (taux_co2 = airSensor.getCO2()) {                           
   if (messageBox == messageInterval) {
       messageBox = 0;
     } else {
       displayScreen.showNumberDecEx(taux_co2, false);    //Affiche les valeurs dans les écrans 7 segements
       messageBox++;
      
    }
  }
}

 
