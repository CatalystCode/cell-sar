#include "dtt.h"

#include <cassert>
#include <cstring>

uint8_t DttMessage::message_id = 0;

DttMessage::DttMessage(uint8_t *buffer, uint32_t buflen) : mode(SEND), error(0) {

   id = DttMessage::message_id;
   DttMessage::message_id = (DttMessage::message_id + 1) % UINT8_MAX;

   cursor = bufstart = (uint8_t *)malloc(sizeof(uint8_t) * buflen);
   bufend = bufstart + buflen;
   memcpy(bufstart, buffer, buflen);

   count = 0;
   total_packets = (bufend - bufstart) / (sizeof(uint8_t) * DTT_MAX_PACKET_DATA_SIZE) + 1;

}

DttMessage::DttMessage() : mode(RECEIVE), error(0) {
   packets = 0;
}

DttMessage::~DttMessage() {
   if (mode == SEND) {

      if (bufstart) 
         free(bufstart);

   } else if (mode == RECEIVE) {
      
      if (packets) {
         for (uint8_t i = 0; i < total_packets; ++i) {
            if (packets[i])
               free(packets[i]);
         }
         free(packets);
      }

   }
}

bool DttMessage::has_next_packet() {
   assert(mode == SEND);
   return cursor < bufend;
}

uint32_t DttMessage::next_packet(uint8_t **packet) {

   assert(mode == SEND);
   if (!has_next_packet()) 
      return 0;

   uint32_t datalen = count < total_packets - 1
      ? DTT_MAX_PACKET_DATA_SIZE : bufend - cursor; 

   uint32_t packet_size = sizeof(uint8_t) * (datalen + DTT_HEADER_LENGTH);
   assert(packet_size <= DTT_MAX_PACKET_SIZE);
   *packet = (uint8_t *)malloc(packet_size);

   (*packet)[DTT_PACKET_ID_FIELD]     = id;
   (*packet)[DTT_PACKET_COUNT_FIELD]  = count;
   (*packet)[DTT_PACKET_TOTAL_FIELD]  = total_packets;
   (*packet)[DTT_PACKET_LENGTH_FIELD] = datalen;

   memcpy(*packet + DTT_PACKET_DATA_FIELD, cursor, datalen);

   count += 1;
   cursor += datalen;

   return datalen + DTT_HEADER_LENGTH;

}

bool DttMessage::is_complete() {
   assert(mode == RECEIVE);

   if (!packets) 
      return false;

   for (uint8_t i = 0; i < total_packets; ++i) {
      if (!packets[i]) 
         return false;
   }

   return true;
}

bool DttMessage::add_packet(uint8_t *packet, uint32_t packetlen) {
   assert(mode == RECEIVE);
   assert(packetlen > DTT_HEADER_LENGTH);

   uint8_t *packet_id = packet + DTT_PACKET_ID_FIELD;
   uint8_t *packet_count = packet + DTT_PACKET_COUNT_FIELD;
   uint8_t *packet_total = packet + DTT_PACKET_TOTAL_FIELD;
   uint8_t *packet_length = packet + DTT_PACKET_LENGTH_FIELD;
   uint8_t *packet_data = packet + DTT_PACKET_DATA_FIELD;

   if (packets == 0) {
      id = *packet_id;
      total_packets = *packet_total;
      packets = (uint8_t **)calloc(sizeof(uint8_t *), total_packets);
   } else {
      if (id != *packet_id) {
         error = "bad packet id";
         return false;
      }
      if (total_packets != *packet_total) {
         error = "bad packet total";
         return false;
      }
      if (packets[*packet_count] != 0) {
         error = "bad packet count";
         return false;
      }
   }

   uint8_t *entry = packets[*packet_count];
   
   entry = (uint8_t *)calloc(sizeof(uint8_t), *packet_length + 1);
   *entry = *packet_length;
   memcpy(entry + 1, packet_data, *packet_length);

   return true;
}

uint32_t DttMessage::get_whole_message(uint8_t **message) {
   assert(mode == RECEIVE);

   if (!packets || !is_complete()) 
      return 0;

   uint32_t total_length = 0;
   for (uint8_t i = 0; i < total_packets; ++i)
      total_length += packets[i][0];

   uint8_t *cur = *message = (uint8_t *)calloc(sizeof(uint8_t), total_length);
   for (uint8_t i = 0; i < total_packets; ++i) {
      memcpy(cur, packets[i] + 1, packets[i][0]);
      cur += packets[i][0];
   }

   return total_length;
}

