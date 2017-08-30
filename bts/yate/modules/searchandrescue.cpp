/** searchandrescue.cpp: a Yate module that handles communication between 
  * the SAR javascript automation and the OCP program.
  *    
  * Cellular Search and Rescue - Cellular Sensor BTS
  *   Copyright (C) 2017 Microsoft
  * Yet Another Telephony Engine - Base Transceiver Station
  *   Copyright (C) 2013-2014 Null Team Impex SRL
  *   Copyright (C) 2014 Legba, Inc
  * 
  * This file is part of cell-sar/the Yate-BTS Project http://www.yatebts.com
  * 
  * cell-sar is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  * 
  * cell-sar is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with cell-sar.  If not, see <http://www.gnu.org/licenses/>.
  */

#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdlib.h>
#include <string>

#include <yatengine.h>
#include <yatescript.h>

#include <google/protobuf/util/json_util.h>
#include "sarqueue.h"
#include "data.pb.h"
#include "from_fc.pb.h"
#include "from_yate.pb.h"

using namespace TelEngine;
using namespace google::protobuf::util;
using namespace com::microsoft::cellsar::protobuf;

class SearchAndRescue : public JsObject {
   YCLASS(SearchAndRescue, JsObject)

public:

   static const char *OUT_FILE;

   SearchAndRescue(Mutex *mtx);

   virtual ~SearchAndRescue();

   virtual SearchAndRescue *runConstructor(ObjList &stack, const ExpOperation &oper, GenObject *context);

   static void initialize(ScriptContext *context);

protected:

   bool runNative(ObjList &stack, const ExpOperation &oper, GenObject *context);

   /**
    * Convert the passed JSON object to a YateMessage protobuffer and pass it to the ocp.
    */
   void write_to_ocp(ObjList &stack, const ExpOperation &oper, GenObject *context);

   /**
    * Convert the FcMessage protobuffer to JSON and return it.
    */
   void read_from_ocp(ObjList &stack, const ExpOperation &oper, GenObject *context);

   void change_plmn(ObjList &stack, const ExpOperation &oper, GenObject *context);

};

class SearchAndRescueHandler : public MessageHandler {

public:

   SearchAndRescueHandler();

   virtual bool received(Message &msg);

};

class SearchAndRescuePlugin : public Plugin {

public:
   
   SearchAndRescuePlugin();

   virtual void initialize();

private:

   SearchAndRescueHandler *m_handler; 
};

/* ######################### IMPLEMENTATION ######################### */

// --- SearchAndRescue ---

const char *SearchAndRescue::OUT_FILE = "/tmp/sar.log";

SearchAndRescue::SearchAndRescue(Mutex *mtx) : JsObject("SearchAndRescue", mtx, true) {
   Debug(DebugAll, "SearchAndRescue::SearchAndRescue(%p) [%p]", mtx, this);
   params().addParam(new ExpFunction("writeToOCP"));
   params().addParam(new ExpFunction("readFromOCP"));
   params().addParam(new ExpFunction("changePLMN"));
}

SearchAndRescue::~SearchAndRescue() {
   Debug(DebugAll, "SearchAndRescue::~SearchAndRescue() [%p]", this);
}

SearchAndRescue *SearchAndRescue::runConstructor(ObjList &stack, const ExpOperation &oper, GenObject *context) {
   Debug(DebugAll, "SearchAndRescue::runConstructor '%s'(" FMT64 ") [%p]", oper.name().c_str(), oper.number(), this);
   const char *val = 0;
   ObjList args;
   switch (extractArgs(stack, oper, context, args)) {
   case 1:
      val = static_cast<ExpOperation*>(args[0])->c_str();
      // fall through
   case 0:
      return new SearchAndRescue(mutex());
   default:
      return 0;
   }
}

void SearchAndRescue::initialize(ScriptContext *context) {
   if (!context) return;

   Mutex *mtx = context->mutex();
   Lock mylock(mtx);
   NamedList &params = context->params();
   if (!params.getParam(YSTRING("SearchAndRescue")))
      addConstructor(params, "SearchAndRescue", new SearchAndRescue(mtx));
   else
      Debug(DebugInfo, "A SearchAndRescue already exists, nothing to do");
}

bool SearchAndRescue::runNative(ObjList &stack, const ExpOperation &oper, GenObject *context) {
   if (oper.name() == YSTRING("writeToOCP"))
      this->write_to_ocp(stack, oper, context);
   else if (oper.name() == YSTRING("readFromOCP"))
      this->read_from_ocp(stack, oper, context);
   else if (oper.name() == YSTRING("changePLMN"))
      this->change_plmn(stack, oper, context);
   return true;
}

void SearchAndRescue::write_to_ocp(ObjList &stack, const ExpOperation &oper, GenObject *context) {
   ObjList args;
   if (extractArgs(stack, oper, context, args) < 1) return;

   // get the json string
   ExpOperation *msg = static_cast<ExpOperation*>(args[0]);
   std::string json(msg->c_str());

   // convert it to a protobuf object
   yate::YateMessage yate_message;
   Status status = JsonStringToMessage(json, &yate_message);

   std::ofstream sar_log;
   sar_log.open(SearchAndRescue::OUT_FILE, std::ios_base::app);
   sar_log << "[WRITE]> " << json.c_str() << std::endl;

   if (status.ok()) {

      // write the protobuf object to the POSIX queue
      unsigned int size = yate_message.ByteSize();
      void *buffer = malloc(size);
      yate_message.SerializeToArray(buffer, size);
      MQCommon::push((const char *)buffer);
      free(buffer);

   } else {
      sar_log << "   ERROR converting json to YateMessage: " << status.ToString() << std::endl;
   } 
   
   sar_log << std::endl;
   sar_log.close();
   ExpEvaluator::pushOne(stack, new ExpOperation(status.ok()));
}

void SearchAndRescue::read_from_ocp(ObjList &stack, const ExpOperation &oper, GenObject *context) {
   String tmp;

   // read from the POSIX queue
   char *buffer;
   unsigned int buflen = MQCommon::pop(&buffer);
   if (buffer == NULL) {
      ExpEvaluator::pushOne(stack, new ExpOperation(tmp));
      return;
   }

   std::ofstream sar_log;
   sar_log.open(SearchAndRescue::OUT_FILE, std::ios_base::app);
   
   // convert the buffer to an FcMessage
   fc::FcMessage fc_message;
   bool result = fc_message.ParseFromArray((const void *)buffer, buflen);

   // convert to a JSON string
   std::string json("");
   Status status;
   if (result) {
      status = MessageToJsonString(fc_message, &json);
   }

   // log results
   sar_log << "[READ ]> " << json << std::endl;
   if (!result) {
      sar_log << "   ERROR parsing FcMessage from array." << std::endl;
   } else if (!status.ok()) {
      sar_log << "   ERROR converting FcMessage to JSON: " << status.ToString() << std::endl;
   }
   sar_log << std::endl;

   sar_log.close();
   String jsonResult(json.c_str());
   ExpEvaluator::pushOne(stack, new ExpOperation(jsonResult));
}

void SearchAndRescue::change_plmn(ObjList &stack, const ExpOperation &oper, GenObject *context) {
   std::string mcc = "";
   std::string mnc = "";
   
   ObjList args;
   int argc = extractArgs(stack, oper, context, args);
   String tmp;
   tmp << "SearchAndRescue: change_plmn ";

   for (int i = 0; i < argc; ++i) {
      ExpOperation *op = static_cast<ExpOperation*>(args[i]);

      switch (i) {
      case 0:
         mcc = std::string(op->c_str());
         tmp << "MCC='" << *op << "' ";
         break;
      case 1:
         mnc = std::string(op->c_str());
         tmp << "MNC='" << *op << "' ";
         break;
      }
   }

   ExpEvaluator::pushOne(stack, new ExpOperation(tmp));

   String chplmn;
   chplmn << "chplmn.sh " << mcc.c_str() << " " << mnc.c_str() << " &";
   system(chplmn.c_str());
}

// --- Handler ---

SearchAndRescueHandler::SearchAndRescueHandler() : 
      MessageHandler("script.init", 90, "searchandrescue") {}

static const Regexp s_libs("\\(^\\|,\\)searchandrescue\\($\\|,\\)");
static const Regexp s_objs("\\(^\\|,\\)SearchAndRescue\\($\\|,\\)");

bool SearchAndRescueHandler::received(Message &msg) {

   ScriptContext *ctx = YOBJECT(ScriptContext, msg.userData());
   const String &lang = msg[YSTRING("language")];
   Debug(DebugInfo, 
      "SearchAndRescueHandler received script.init, language %s, context: %p", 
      lang.c_str(), ctx);

   if (!ctx || (lang && lang != YSTRING("javascript"))) 
      return false;

   bool ok = msg.getBoolValue(YSTRING("startup")) ||
      s_libs.matches(msg.getValue(YSTRING("libraries"))) ||
      s_objs.matches(msg.getValue(YSTRING("objects")));

   if (ok) SearchAndRescue::initialize(ctx);
   return ok;
   
}

// --- Plugin ---

SearchAndRescuePlugin::SearchAndRescuePlugin() :
      Plugin("searchandrescue", true), m_handler(0) {
   Output("Hello, I am module SearchAndRescuePlugin");
}

void SearchAndRescuePlugin::initialize() {
   Output("Initializing SearchAndRescuePlugin");
   if (!m_handler)
      Engine::install((m_handler = new SearchAndRescueHandler));
}

INIT_PLUGIN(SearchAndRescuePlugin);

