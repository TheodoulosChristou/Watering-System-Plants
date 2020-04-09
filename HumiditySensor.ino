

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"
#define DHTPIN 2
#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <virtuabotixRTC.h>



// Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.


DHT dht(DHTPIN, DHTTYPE);
int water_pump = 6;
int water_level_sensor = 7;
int soilMoisturePin = A0;
RF24 radio(9, 10);

byte pipe[][6] = {"1Node", "2Node"};

boolean radioNumber = 1;

typedef struct packet_t {
  float h;
  double c;
  double f;
  double hic;
  double hif;
  double water_value;
  int sensorValue;
} Packet;
virtuabotixRTC myRtc(A1,A2,A3);
String keyword = "Aloma_23u!";

void setup() {
  Serial.begin(9600);
  pinMode(soilMoisturePin, INPUT);
  password();
  
  if (radio.begin()) {
    Serial.println("NRF24 initialised");
  }
  // radio.setChannel(115);
  radio.setPALevel(RF24_PA_LOW);
  //  radio.setDataRate(RF24_250KBPS);
  if (radioNumber) {
    radio.openReadingPipe(1, pipe[0]);
    radio.openWritingPipe(pipe[1]);
  } else {
    radio.openReadingPipe(1, pipe[1]);
    radio.openWritingPipe(pipe[0]);
  }

  radio.stopListening();
  
  pinMode(water_pump, OUTPUT);
  Serial.println("The system will begin in....");
  countDownSystem(5);
  Serial.println(F("Demo Testing!"));
  dht.begin();
  
  //seconds,minutes,hours,day of the week, day of the month,month,year
  myRtc.setDS1302Time(15, 53, 1, 3, 1, 4, 2020);
}

void loop() {
  // Wait a few seconds between measurements.
  
  delay(1000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

   radio.stopListening();

   Packet packet;
   packet.h = humidity();
   packet.c = temperatureCelsius();
   packet.f = temperatureFarheneit();
   packet.hic = heatIndexCels();
   packet.hif = heatIndexCels();
   packet.water_value = water_level_sensor_value();
   packet.sensorValue = map(analogRead(soilMoisturePin),0,1023,0,100);

  // Check if any reads failed and exit early (to try again).

    if (isnan(packet.h) || isnan(packet.c) || isnan(packet.f)){
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    
  if ((packet.c < 40) || (packet.h < 100)){
     digitalWrite(water_pump, HIGH);
  } else {
    digitalWrite(water_pump, LOW);
    Serial.println("Serial Monitor reached the highest temperature");
  }
  if ((packet.sensorValue < 200)) {
    digitalWrite(water_pump, HIGH);
  } else {
    digitalWrite(water_pump, LOW);
    Serial.println("Serial Monitor reached the highest soil moisture level");
  }

  if (packet.water_value <= 100.0) {
    digitalWrite(water_pump, HIGH);
  } else {
    digitalWrite(water_pump, LOW);
    Serial.println("Water Level reached the highest value");
  }

  radio.write(&packet, sizeof(Packet)); 
  Serial.print(F("Humidity: "));
  Serial.print(packet.h);
  Serial.print(F("% Temperature: "));
  Serial.print(packet.c);
  Serial.print(F("°C "));
  Serial.print(packet.f);
  Serial.print(F("°F  "));
  Serial.print("Moisture Level: ");
  Serial.print(packet.sensorValue);
  Serial.print(" % ");
  Serial.print(F(" Heat index: "));
  Serial.print(packet.hic);
  Serial.print(F("°C "));
  Serial.print(packet.hif);
  Serial.print(F(" °F"));
  Serial.println(" ");
  Serial.print("Water Level: ");
  Serial.print(packet.water_value);
  Serial.print(" % ");
  Serial.println(" ");

   myRtc.updateTime();
   Serial.print("Current Date / Time: ");
   Serial.print(myRtc.dayofmonth); //You can switch between day and month if you're using American system
   Serial.print("/");
   Serial.print(myRtc.month);
   Serial.print("/");
   Serial.print(myRtc.year);
   Serial.print(" ");
   Serial.print(myRtc.hours);
   Serial.print(":");
   Serial.print(myRtc.minutes);
   Serial.print(":");
   Serial.print(myRtc.seconds);
   Serial.println(" ");
}


void countDownSystem(int num) {
  for (int i = num;  i > 0; i--) {
    Serial.print(i);
    delay(100);
    Serial.print(",");
  }
  Serial.println("\n");
  Serial.println("Begin the system");
}

float humidity() {
  float hum = dht.readHumidity();
  return hum;
}

double temperatureCelsius() {
  double cels = dht.readTemperature();
  return cels;
}

double temperatureFarheneit() {
  double far = dht.readTemperature(true);
  return far;
}

double heatIndexFar() {
  double hif = dht.computeHeatIndex(temperatureFarheneit(), temperatureCelsius());
  return hif;
}

double heatIndexCels() {
  double hic = dht.computeHeatIndex(temperatureCelsius(), temperatureFarheneit(), false);
  return hic;
}

double water_level_sensor_value() {
  double v = analogRead(water_level_sensor) / 100.0;
  return v;
}


void password(){
  Serial.println("Enter your password: ");
  while(Serial.available() == 0){
  }
  String text;
  text = Serial.readString();
  text.trim();
  int attempt = 0;
  if(text.equals(keyword)){
    Serial.println("Your password is correct");
  }else{
    Serial.println("Your Password is incorrect Try again");
    password();
  }
}
