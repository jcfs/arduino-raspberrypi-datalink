#include <SoftwareSerial.h>
#include "PhysicalLayer.h"
#include "DataLinkLayer.h"

#define TX_PIN 11
#define RX_PIN 10

PhysicalLayer Physical = PhysicalLayer(TX_PIN, RX_PIN, NULL, 9600);
DataLinkLayer DataLink = DataLinkLayer(&Physical, 0x1);


void setup()  
{
    Serial.begin(9600);

    Frame * frame = new Frame();

}

void loop() // run over and over
{
    DataLink.process();
}
