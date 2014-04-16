#include "PhysicalLayer.h"
#include "DataLinkLayer.h"
#include "Frame.h"
#include "Packet.h"
#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <iostream>
#include <pthread.h>

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

void *  read_input(void * cmd) {
    cout << "Commad: ";    
    string *tmp = (string*)(cmd);
    getline(cin,*tmp);
}



int main(int argc, char ** argv) {

    wiringPiSetupGpio();

    PhysicalLayer Physical = PhysicalLayer(TX_PIN, RX_PIN, "/dev/ttyAMA0", 9600);
    DataLinkLayer DataLink = DataLinkLayer(&Physical, 0x0);

    pthread_t client_thread;
    std::string cmd;

    pthread_create(&client_thread, NULL, read_input, &cmd);

    Packet * packet = new Packet();

    while(true) {
        if (cmd != "") {
            DataLink.write(0x1, packet);
            cmd = "";
            cout << "========================" << endl;
            DataLink.process();
            cout << "========================" << endl;
            pthread_create(&client_thread, NULL, read_input, &cmd);
        } else {
            DataLink.process();
        }
    }


}


