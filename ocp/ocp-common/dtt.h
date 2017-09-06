#pragma once
#ifndef _DTT_H
#define _DTT_H

#include <stdint.h>
#include <stdlib.h>

#define DTT_HEADER_LENGTH                4
#define DTT_MAX_PACKET_SIZE            100
#define DTT_MAX_PACKET_DATA_SIZE   100 - 4

#define DTT_PACKET_ID_FIELD      0
#define DTT_PACKET_COUNT_FIELD   1
#define DTT_PACKET_TOTAL_FIELD   2
#define DTT_PACKET_LENGTH_FIELD  3
#define DTT_PACKET_DATA_FIELD    4

enum DttMessageMode { SEND, RECEIVE };

class DttMessage {

private:
   
   static uint8_t message_id;

public:

   DttMessage(uint8_t *buffer, uint32_t buflen);
   DttMessage();
   ~DttMessage();

   inline uint8_t get_id() { return id; }

   /* Only for mode == SEND */
   bool has_next_packet();
   uint32_t next_packet(uint8_t **packet);

   /* Only for mode == RECEIVE */
   bool is_complete();
   bool add_packet(uint8_t *packet, uint32_t packetlen);
   uint32_t get_whole_message(uint8_t **message);

private:

   DttMessageMode mode;

   uint8_t id;
   uint8_t total_packets;
   const char *error;

   /* Only for mode == SEND */
   uint8_t *bufstart;
   uint8_t *bufend;
   uint8_t *cursor;
   uint8_t count;

   /* Only for mode == RECEIVE */
   uint8_t **packets;

};

#endif

