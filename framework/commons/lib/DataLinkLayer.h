/*
   Half-duplex 1-wire connections between multiple Arduino and RPi
   Copyright (C) 2014  Joao Salavisa (joao.salavisa@gmail.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DATALINKLAYER_H
#define DATALINKLAYER_H

#include "PhysicalLayer.h"
#include "Packet.h"
#include "Frame.h"
#include <stdint.h>

#define DEFAULT_PROCCESS_INTERVAL	 100 /* Delay for each process method call */  
#define FRAME_TX_MAX_RETRY		   4 /* Max retrys for a non-acked frame to be transmitted */
#define FRAME_ACK_WAIT_TIMEOUT		1000 /* Timeout to wait for an ack on a frame before retrying */

#define FRAME_START_BYTE		0xFF

class DataLinkLayer {
    private:
        // Physical layer instance
        PhysicalLayer * _physicalLayer;			
        // The address that will identify this DataLinkLayer instance
        uint8_t _address;
        // Interval to run the process method
        uint32_t _process_interval;
        //List of the frames that have been successfully received and havent been read or acked yet
        Frame ** _receiveFrameList;
        //List of the frames that are queued to be transmited
        Frame ** _transmitFrameList;
        uint32_t _rx_buffer_size;
        uint32_t _tx_buffer_size;

        uint8_t * _current_rx_buffer;
        uint8_t _current_rx_buffer_size;
        // Map with the current ids to be used in the transmit frames
        // The key is the receiver address
        uint32_t _transmitIds;
        //Function that given a buffer of data creates the frames needed accordingly 
        Frame ** split_data_into_frames(uint8_t address, uint8_t * data, uint32_t size, uint8_t * number_of_frames);
        // remove frame from tx queue
        void remove_frame_from_tx_queue(Frame * frame);
        // Function that queues the given frames in the tx list
        void queue_frames_to_tx(Frame ** frames, uint8_t number_of_frames);
        // Function that process a incoming frame
        void process_rx_frame(Frame * frame);

        // Get frames of the given sequence number
        Frame ** get_frames_of_sequence(uint32_t sequence_number, uint32_t * n_frames);
        //Function that creates an ack for the frame passed
        Frame * build_ack_frame(uint8_t address, Frame * frame);
        //Encode the frame data to pass to the physical layer	
        uint8_t * frame_encode(Frame * frame, uint8_t * size);
        //Decode the data from the physical layer to a frame
        Frame * frame_decode(uint8_t * data, uint32_t size);
        // checks the frame to be read
        bool should_read_frame(Frame * frame);
        // Call to the physical layer in order to write the the transmition queue
        void writeData();
        // Call to the physical layer in order to read to the receive queue
        void readData();
    public:
        DataLinkLayer(PhysicalLayer * physicalLayer, uint8_t address);
        // Writes a packet	
        uint32_t write(uint8_t address, Packet * packet);	
        // Reads the available packets and sets the amount of packets read
        Packet ** read(uint32_t * packets_read);
        // method that must be called to update the frame status and delegate them to the
        // physical layer
        bool process();

        //Relevant setters
        void set_process_interval(uint32_t process_interval) { _process_interval = process_interval; }
};



#endif /* DATALINKLAYER_H */
