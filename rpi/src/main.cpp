#include "PhysicalLayer.h"
#include "DataLinkLayer.h"
#include "Frame.h"
#include "Packet.h"
#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>


using namespace std;

#define GET_BIT(data, index) ((*(data + (index / 8))) >> (8 - (index % 8) - 1) & 0x1)

#define TX_PIN 14
#define RX_PIN 15

void print_bits(char * buffer, int size) {
	for(int i = 0; i < size * 8; i++) {
		printf("%d", GET_BIT(buffer, i));
	}
	printf("\n");
}

int main(int argc, char ** argv) {
	wiringPiSetupGpio();
	PhysicalLayer Physical = PhysicalLayer(TX_PIN, RX_PIN, "/dev/ttyAMA0", 9600);
	DataLinkLayer DataLink = DataLinkLayer(&Physical, 0x0);

    printf("%d\n", sizeof(class Frame));

	Packet * packet = new Packet();
	DataLink.write(0x1, packet);

	while(true) {
		DataLink.process();
	}
}


