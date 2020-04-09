#include <SPI.h>
#include <RF24.h>


RF24 radio(9, 10);
byte pipe[][6] = {"1Node", "2Node"};
boolean radioNumber = 0;
typedef struct packet_t {
  double h;
  double c;
  double f;
  double hic;
  double hif;
  double water_value;
  int sensorValue;
} Packet;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if(radio.begin()) {
//    Serial.println("NRF24 initialised");
    }
  // radio.setChannel(115);
   radio.setPALevel(RF24_PA_LOW);
  //  radio.setDataRate(RF24_250KBPS);
  if (radioNumber) {
    radio.openReadingPipe(1, pipe[0]);
  } else {
    radio.openReadingPipe(1, pipe[1]);
  }
  radio.startListening();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (radio.available()) {
    Packet readPacket; 
    radio.read(&readPacket, sizeof(Packet));
    Serial.print(readPacket.h);
    Serial.print(" , ");
    Serial.print(readPacket.c);
    Serial.print(" , ");
    Serial.print(readPacket.f);
    Serial.print(" , ");
    Serial.print(readPacket.sensorValue);
    Serial.print(" , ");
    Serial.print(readPacket.hic);
    Serial.print(" , ");
    Serial.print(readPacket.hif);
    Serial.print(" , ");
    Serial.print(readPacket.water_value);
    Serial.println(" ");
   delay(1000);

  }
}
