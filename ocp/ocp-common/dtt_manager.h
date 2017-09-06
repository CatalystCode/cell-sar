#pragma once
#ifndef _DTT_MANAGER_H
#define _DTT_MANAGER_H

#include <stdint.h>

#include "dtt.h"

class DttManager {

public:

   static uint8_t parse_id(uint8_t *buffer, uint32_t buflen);

   DttManager();
   ~DttManager();

   bool has(uint8_t id);
   DttMessage *get(uint8_t id);
   DttMessage *add(uint8_t id);
   void remove(uint8_t id);

private:

   DttMessage *dtt_messages[UINT8_MAX]; 

};

#endif
