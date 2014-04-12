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
#include "DataLinkLayer.h"
#include "FrameCrc.h"
#include "PrintUtil.h"
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>

#define GET_BIT(data, index) ((*(data + (index / 8))) >> (8 - (index % 8) - 1) & 0x1)

DataLinkLayer::DataLinkLayer(PhysicalLayer * physicalLayer, uint8_t address) {
    _physicalLayer = physicalLayer;
    _address = address;
    _process_interval = DEFAULT_PROCCESS_INTERVAL;
    _receiveFrameList = NULL;
    _transmitFrameList = NULL;
    _tx_buffer_size = 0;
    _rx_buffer_size = 0;
    _current_rx_buffer = NULL;
    _current_rx_buffer_size = 0;
}

// Adds new frames to the transmit frame list
// The Packets is first splitted into frames, those frames are then added to the end of the frame list
uint32_t DataLinkLayer::write(uint8_t address, Packet * packet) {

    uint8_t number_of_frames;
    Frame ** frames = split_data_into_frames(address, (uint8_t *) packet, sizeof(*packet), &number_of_frames);
    queue_frames_to_tx(frames, number_of_frames);

    return number_of_frames;
}


// Reads the amount of packets available to be read from the rx queue, and remove them after being
// read
// Returns all the packets that are in state to be read
Packet ** DataLinkLayer::read(uint32_t * packets_read) {

    Packet ** result = NULL;

    for(int i = 0; i < _rx_buffer_size; i++) {
        uint32_t n_frames = 0;
        Frame ** frames;// = get_frames_of_sequence(_receiveFrameList[i]->get_seq_number(), &n_frames);
        if (frames != NULL) {
           for(int j = 0; j < n_frames; j++) {

           } 
        }
    }


}

bool DataLinkLayer::process() {
    writeData();
    readData();
}

// Private methods
// Writes data from the transmit queue to the physical layer and sets the frames written as transmited
void DataLinkLayer::writeData() {
    for(int i = 0; i < _tx_buffer_size; i++) {
        Frame * frame = _transmitFrameList[i];
        if (!(frame->get_control_data() & CONTROL_TRANS_FRAME)) {
            uint8_t encoded_frame_size = 0;
            frame->prepare_to_transmit();
            uint8_t * encoded_frame = frame_encode(frame, &encoded_frame_size);
            uint32_t bytes_written = _physicalLayer->write(encoded_frame, encoded_frame_size);
            PrintUtl.prints("Bytes written %d\n\r", bytes_written);
            if (bytes_written != 0) {
                frame->set_control_data(frame->get_control_data() | CONTROL_TRANS_FRAME);

            }
            PrintUtl.prints("Sent ");
            frame->print();
            free(encoded_frame);
        }
    }
}

void DataLinkLayer::process_rx_frame(Frame * frame) {
    if (!(frame->get_control_data() & CONTROL_ACK_FRAME)){
        // if it is not an ack frame we replicate it and add it to the tx queue
        Frame * ack_frame = new Frame();

        memcpy(ack_frame, frame, sizeof(Frame)); 

        uint8_t source = frame->get_src_address();

        ack_frame->set_src_address(_address);
        ack_frame->set_dst_address(source);

        ack_frame->set_control_data((ack_frame->get_control_data() | CONTROL_ACK_FRAME) & ~CONTROL_TRANS_FRAME);
        queue_frames_to_tx(&ack_frame, (uint8_t)1);

    } else {
        // it is an ack frame we'll run through the tx queue and mark it acked
        remove_frame_from_tx_queue(frame);
    }
}

// Remove a frame from the tx queue
void DataLinkLayer::remove_frame_from_tx_queue(Frame * frame) {
    PrintUtl.prints("inside remove frame from txt queue\n\r");
    Frame ** newTxQueue = (Frame **)calloc(_tx_buffer_size, sizeof(*frame));

    int amount_freed = 0;
    for(int i = 0, j = 0; i < _tx_buffer_size; j++, i++) {
        Frame * current_frame = _transmitFrameList[i];
        if (current_frame->get_frame_id() == frame->get_frame_id() && current_frame->get_frame_seq() == frame->get_frame_seq()) {
            free(current_frame);
            amount_freed++;
            j--;
        } else {
            newTxQueue[j] = current_frame;
        }
    }

    free(_transmitFrameList);
    _transmitFrameList = newTxQueue;
    _tx_buffer_size -= amount_freed;

    PrintUtl.prints("%d %d\n\r", amount_freed, _tx_buffer_size);
}

// Reads data from the physical layer decodes it to frames and adds it to the receive queue
void DataLinkLayer::readData() {
    uint32_t available_bytes = _physicalLayer->available();

    if (available_bytes) {
        if (_current_rx_buffer == NULL) {
            _current_rx_buffer = (uint8_t*)calloc(available_bytes, sizeof(uint8_t));
        } else {
            _current_rx_buffer = (uint8_t*)realloc(_current_rx_buffer, (_current_rx_buffer_size + available_bytes) * sizeof(uint8_t));
        }

        _physicalLayer->read(_current_rx_buffer + _current_rx_buffer_size, available_bytes);
        _current_rx_buffer_size += available_bytes;

        PrintUtl.prints("Current rx buffer size %d %d\r\n", available_bytes, _current_rx_buffer_size);
    }

    if (_current_rx_buffer_size >= sizeof(Frame)) {
        for(int i = 0; i < _current_rx_buffer_size; i++) {
            if (_current_rx_buffer[i] == FRAME_START_BYTE) {
                Frame * frame = frame_decode(_current_rx_buffer + i, _current_rx_buffer_size - i);
                // Only accept valid frames and from other clients
                // to us
                if (Crc.validate(frame)) {
                    if (should_read_frame(frame)) {

                        Frame ** newRxFrameList = (Frame **)realloc(_receiveFrameList, (_rx_buffer_size + 1) * sizeof(Frame *));
                        if (newRxFrameList == NULL) {
                            //BOOM!! We were not able to allocate memory
                            return;
                        } else if (newRxFrameList != _receiveFrameList) {
                            _receiveFrameList = newRxFrameList;
                        }
                        _receiveFrameList[_rx_buffer_size] = frame;
                        _rx_buffer_size++;


                        process_rx_frame(frame);
                        frame->print();
                    }
                    free(_current_rx_buffer);
                    _current_rx_buffer_size = 0;
                } else {
                    free(frame);
                }
            }
        } 

    }
}

bool DataLinkLayer::should_read_frame(Frame * frame) {
    return Crc.validate(frame) && frame->get_src_address() != _address && frame->get_dst_address() == _address;
}

// Queues <>number_of_frames<> in the _transmitFrameList
void DataLinkLayer::queue_frames_to_tx(Frame ** frames, uint8_t number_of_frames) {
    Frame ** newTxFrameList = (Frame **)realloc(_transmitFrameList, (_tx_buffer_size + number_of_frames) * sizeof(Frame *));
    if (newTxFrameList == NULL) {
        //Boom! We were not able to allocate what we needed
        return;
    } else if (newTxFrameList != _transmitFrameList) {
        _transmitFrameList = newTxFrameList;
    }

    for(int i = _tx_buffer_size; i < _tx_buffer_size + number_of_frames; i++) {
        _transmitFrameList[i] = frames[i - _tx_buffer_size];
    } 

    _tx_buffer_size += number_of_frames;

}

// Creates the frames needed to accomodate the given data
Frame ** DataLinkLayer::split_data_into_frames(uint8_t address, uint8_t * data, uint32_t size, uint8_t * number_of_frames) {
    *number_of_frames = size / PAYLOAD_MAX_SIZE + (size % PAYLOAD_MAX_SIZE != 0 ? 1 : 0);
    Frame ** result = (Frame **) calloc(*number_of_frames, sizeof(Frame));

    //Get the if of the last frame sent to the passed address and increment it to use it
    uint32_t frame_id = ((address << 24) & 0xFF000000) | _transmitIds++;    

    for(int frame_seq = 0; frame_seq < *number_of_frames; frame_seq++) {
        uint8_t control_data = 0;

        control_data |= (!frame_seq ? CONTROL_FIST_FRAME : 0);

        if(frame_seq == (*number_of_frames - 1)) {
            control_data |= CONTROL_LAST_FRAME;
        }

        Frame * frame = new Frame(frame_id, frame_seq, _address, address, control_data, data, PAYLOAD_MAX_SIZE);
        result[frame_seq] = frame;
    }   

    return result;
}


// Encodes a frame according to the following rule:
// First byte : Start sequence = 1111 1111 (0xFF)
// Second byte : Total size of the encoded frame
// The starting sequence cannot be repeated in the rest of the encoded frame, so 
// a 0 (zero) is interleaved every 7 consecutive 1s (ex: 1111 1111 is encoded to 1111 1110 1000)
uint8_t * DataLinkLayer::frame_encode(Frame * frame, uint8_t * size) {
    uint8_t * data = (uint8_t *) frame;
    uint32_t consecutive_ones = 0;
    uint32_t total_extra_bits = 0;  
    uint32_t bit_number = 0;

    // Calculate the necessary extra bits to encode the frame
    for(int i = 0; i < sizeof(*frame) * 8; i++) {
        if (GET_BIT(data, i)) {
            consecutive_ones++;
            if (consecutive_ones == 8) {
                total_extra_bits++;
                consecutive_ones = 1;
            }
        } else {
            consecutive_ones = 0;
        }
    }

    // Allocate the necessary space for the encoded frame
    // The size the total size of the frame plus the necessary bytes for the extra bits
    uint32_t alloc_size = sizeof(*frame) + (total_extra_bits ? (total_extra_bits / 8) + 1 : 0);
    uint8_t * result = (uint8_t *) calloc(alloc_size + 2, sizeof(uint8_t));

    // Set the frame start byte 
    *result = FRAME_START_BYTE;
    // Set the total allocation size
    *(result + 1) = alloc_size;
    consecutive_ones = 0;

    // Encodes the frame interleaving a zero every 7 consecutive ones   
    for(int i = 0; i < sizeof(*frame) * 8; i++, bit_number++) {
        uint8_t current_bit = GET_BIT(data, i);
        if (current_bit) {
            consecutive_ones++;
            if (consecutive_ones == 8) {
                bit_number++;
                consecutive_ones = 1;
            } 
        } else {
            consecutive_ones = 0;
        }   
        *(result + 2 + (bit_number / 8)) |= (current_bit << (8 - (bit_number % 8) - 1));
    }

    *size = alloc_size + 2;
    return result;
}


Frame * DataLinkLayer::frame_decode(uint8_t * data, uint32_t size) {
    Frame * frame = new Frame();
    uint8_t * pointer = (uint8_t *) frame;
    uint8_t data_size = 0;
    uint32_t consecutive_ones = 0;
    int bit_number = 0;

    // Validate start byte
    if (*data != FRAME_START_BYTE) {
        return 0;
    } else {
        data++; 
    }

    // data size in bytes
    data_size = *data++;
    PrintUtl.prints("data size: %d buffer size %d\r\n", data_size, size);
    if (data_size > sizeof(Frame) + (sizeof(Frame)/8)) {
        return NULL;
    }

    if (data_size > size) {
        // we need to read more bytes from the physical layer
        return NULL;
    }

    for(int i = 0; bit_number < data_size * 8; i++, bit_number++) {
        uint8_t current_bit = GET_BIT(data, i);
        if (current_bit) {
            consecutive_ones++;
            if (consecutive_ones == 7) {
                i++;
                consecutive_ones = 0;
            }
        } else {
            consecutive_ones = 0;
        }

        *(pointer + (bit_number / 8)) |= (current_bit << (8 - (bit_number % 8) - 1));
        PrintUtl.prints("%d", current_bit);
    }
    PrintUtl.prints("\r\n");
    uint8_t * p = (uint8_t *) pointer;

    return frame;
}

