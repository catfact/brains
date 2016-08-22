#ifndef _SLABS_SERIAL_PROTOCOL_H_
#define _SLABS_SERIAL_PROTOCOL_H_

#include <stdint.h>


// our samples are up to 24 bits
// we'll use 21 bits, split into 3 bytes,
// top bit in payload always zero.

// total number of channels
#define NUM_CHANNELS 72
#define PACKETS_PER_FRAME 4

#define BYTES_PER_SAMPLE 3
#define CHANNELS_PER_PACKET 18
#define PACKET_DATA_BYTES (BYTES_PER_SAMPLE * CHANNELS_PER_PACKET)
#define SOF_VALUE 0b10101010
#define EOF_VALUE 0b10101111

struct packet {
  // start-of-frame marker
  uint8_t sof;
  // index of first channel in this packet
  uint8_t startChannel;
  uint8_t data[PACKET_DATA_BYTES];
  // end-of-frame marker
  uint8_t eof;
};

typedef union {
  struct packet packetStruct;
  uint8_t byteArray[sizeof(struct packet)];
} packet_t;


#define PACKET_SIZE sizeof(struct packet)
#endif
