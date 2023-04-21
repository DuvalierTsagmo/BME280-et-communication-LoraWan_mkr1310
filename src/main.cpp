/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LoRa.h>


#define SEALEVELPRESSURE_HPA (1013.25)

// Fonction 
void printValues(); 

// Declaration object 
Adafruit_BME280 bme; // I2C
Adafruit_BME280 bme2;

// Declaration variables 
int BmeChoisi = 1; 
float Temperature, Humidity, Pression; 

unsigned long TempsAvant, TempsActuel;
const int DelayTime = 1000; 

void setup() {

    Serial.begin(9600);
    Serial.println(F("BME280 test"));
    
    // communication LoraWan
    if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
    }
    Serial.println("-- Default Test --");
    
    Serial.println();
}


void loop() { 
   
   // Apppel la fonction chaque une seconde 
   if (millis() - TempsAvant > DelayTime)
   {
      printValues(); // Appel la fonction printValue() pour capteur et envoie de donnees 
      TempsAvant = millis();
   }
  
}


void printValues() {

    unsigned status, status2;
    
    // default settings
    status = bme.begin(0x76); // Debut de communication I2c avec le bme 1 sur l'adresse 0x76
    
    // Si le bme 1 me fonctionne pas on le change avec le bme numero 2 
    if (!status) {
      
        Serial.println("data from second capteur"); 
        status2 = bme2.begin(0x77); //  Debut de communication I2c avec le bme 2 sur l'adresse 0x77 
        BmeChoisi = 2;  // variable pour la condition de changement vers le bme 2 

        if (!status2) {
          Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");  
        }
        
    }else
        BmeChoisi = 1; 
    
    
    if (BmeChoisi == 1) // Capteur des donnees par le bme numero 1 si il arret de fonctionner on va lancer le bme numero 2 
    {
        Temperature = bme.readTemperature();
        Humidity = bme.readHumidity(); 
        Pression = bme.readPressure() / 100.0F; 

    }else if (BmeChoisi == 2)   // Si le Bme 1 est arret de fontionner le bme numero 2 se met en action pour capter les donnees 
    {
        Temperature = bme2.readTemperature();
        Humidity = bme2.readHumidity(); 
        Pression = bme2.readPressure() / 100.0F; 
    }

  
    // Affichage des donnees sur le moniteur serie 
    Serial.print("Temperature = ");
    Serial.print(Temperature);
    Serial.println(" °C");

    Serial.print("Pressure = ");
    Serial.print(Pression);
    Serial.println(" hPa");

    Serial.print("Humidity = ");
    Serial.print(Humidity);
    Serial.println(" %");

    Serial.println();

    // Envoie de packet lorawan et meme temps la construction de la chaine MQTT 
    // afin d'utiliser cette chaine pour l'envoie sur ThingsBoard 

    LoRa.beginPacket();  // Demarrage communication LoraWan

    LoRa.print("{\"Temperature\": ");
    LoRa.print(Temperature);   // Envoi donnees temperature 
    LoRa.print(",\"Humidity\": ");
    LoRa.print(Humidity);  // Envoi donnees Humidity
    LoRa.print(",\"Pression\": ");
    LoRa.print(Pression);   // Envoi donnees Pression 
    LoRa.print("}");
    
    LoRa.endPacket(); // Fin communication Lorawan 

}

