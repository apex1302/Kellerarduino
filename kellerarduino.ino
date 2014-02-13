// Written by Mark Kingery (Mark(AT)Kingery.de), GPLV3
// DHT22 Library written by ladyada, public domain
//https://github.com/adafruit/DHT-sensor-library

// Abs. Feuchte=13,233*rel.Feuchte*((10^((7,5*T)/(237+T)))/(273,16+T))
//T=Lufttemperatur
// Sensor 1 indoor
// Sensor 2 outdoor
// this program may not be good looking but I`m staisfied

#include "DHT.h"
#include <math.h>
#include <SPI.h> // needed in Arduino 0019 or later
#include <Ethernet.h>
#include <Twitter.h>
#define DHTPIN1 6   // what pin we're connected to
#define DHTPIN2 7   // what pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
// Ethernet Shield Settings
byte mac[] = { 0x00, 0x80, 0x41, 0x3F, 0x1E, 0xED }; //Robotron MAC Adresse

// If you don't specify the IP address, DHCP is used(only in Arduino 1.0 or later).
byte ip[] = { 192, 168, 1, 10 };

// Your Token to Tweet (get it from http://arduino-tweet.appspot.com/)
Twitter twitter("xxx"); //xxx durch eigenen Token ersetzen


// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
int led = 5;
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

char tmp[10];
char tosend[140];

void setup() {
  Serial.begin(9600);
  Serial.println("DHTxx test!");
 pinMode(led, OUTPUT);
 
  dht1.begin();
  dht2.begin();
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h1 = dht1.readHumidity();
  int t1 = dht1.readTemperature();
float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();
float hA1 = 0;
 float hA2 = 0;
 
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t1) || isnan(h1)) {
    Serial.println("Failed to read from DHT");
  } else {
    
  double x = 0;
  double y = 0;
  double z = 0;
  x= 13.233*h1;
  y=7.5*t1;
  z = 237+t1;
  y = y/z;
 float m = pow(10,y);
  z = 237+t1;
  m=m/z;
 
  hA1=m*x;
    Serial.print("Humidity1: ");
    Serial.print(h1);
    Serial.print(" %\t");
    Serial.print("Temperature1: ");
    Serial.print(t1);
    Serial.println(" *C");
    Serial.print("HumidityA1: ");
    Serial.print(hA1);
    Serial.println(" g");
  }
 
 
 
 
 
  if (isnan(t2) || isnan(h2)) {
    Serial.println("Failed to read from DHT");
  } else {
   
  double x = 0;
  double y = 0;
  double z = 0;
  x= 13.233*h2;
  y=7.5*t2;
  z = 237+t2;
  y = y/z;
 float m = pow(10,y);
  z = 237+t2;
  m=m/z;
 
  hA2=m*x;
    Serial.print("Humidity2: ");
    Serial.print(h2);
    Serial.print(" %\t");
    Serial.print("Temperature1: ");
    Serial.print(t2);
    Serial.println(" *C");
    Serial.print("HumidityA2: ");
    Serial.print(hA2);
    Serial.println(" g");
  }
 
 

  if (hA1 > hA2)
  {
  Serial.println(" fan on !");
  strcat (tosend, "fan K1 on !!! ");
  digitalWrite(led, LOW);   // turn the LED on (HIGH is the voltage level)
  } else {
    Serial.println(" fan off !");
    strcat (tosend, "fan K1 off !!! ");
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  }
  //strcat (tosend, "                                                                                                                                        ");
 
  strcat (tosend, "Temp. inside ");
  itoa((int)t1, tmp, 10); strcat (tosend, tmp);
  strcat (tosend, "°c ");
  strcat (tosend, "Hum. inside ");
  itoa((float)h1, tmp, 10); strcat (tosend, tmp);
  strcat (tosend, "% ");
  strcat (tosend, "H2O ");
  itoa((float)hA1, tmp, 10); strcat (tosend, tmp);
  strcat (tosend, "g/m^3 ");
 
    strcat (tosend, "Temp. outside ");
  itoa((float)t2, tmp, 10); strcat (tosend, tmp);
  strcat (tosend, "°c ");
  strcat (tosend, "Hum. outside ");
  itoa((float)h2, tmp, 10); strcat (tosend, tmp);
  strcat (tosend, "% ");
    strcat (tosend, "H2O ");
  itoa((float)hA2, tmp, 10); strcat (tosend, tmp);
  strcat (tosend, "g/m^3 ");
  Serial.print(tosend);
  delay(86400);//1000 mal am Tag Twittern
  //delay(10000);
  Ethernet.begin(mac, ip);
  // or you can use DHCP for autoomatic IP address configuration.
  // Ethernet.begin(mac);
 
  if (twitter.post(tosend)) {
    // Specify &Serial to output received response to Serial.
    // If no output is required, you can just omit the argument, e.g.
    // int status = twitter.wait();
    int status = twitter.wait(&Serial);
    if (status == 200) {
      Serial.println("OK.");
    } else {
      Serial.print("failed : code ");
      Serial.println(status);
    }
  } else {
    Serial.println("connection failed.");
  }
 tosend[0] = '\0';
}
