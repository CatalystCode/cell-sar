#include "dtt_manager.h"

#include <cassert>

uint8_t DttManager::parse_id(uint8_t *buffer, uint32_t buflen) {
   assert(buflen >= 1);
   return buffer[0];
}

DttManager::DttManager() {
   for (uint8_t i = 0; i < UINT8_MAX; ++i)
      dtt_messages[i] = 0;
}

DttManager::~DttManager() {
   for (uint8_t i = 0; i < UINT8_MAX; ++i) {
      if (dtt_messages[i])
         delete dtt_messages[i];
   }
}

bool DttManager::has(uint8_t id) {
   return dtt_messages[id] != 0;
}

DttMessage *DttManager::get(uint8_t id) {
   return dtt_messages[id];
}

DttMessage *DttManager::add(uint8_t id) {
   if (has(id))
      remove(id);

   return dtt_messages[id] = new DttMessage();
}

void DttManager::remove(uint8_t id) {
   if (has(id))
      delete dtt_messages[id];
}
