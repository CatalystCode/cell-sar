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

#include "mqcommon.h"

using namespace TelEngine;

class SearchAndRescue : public JsObject {
   YCLASS(SearchAndRescue, JsObject)

public:

   static const char *OUT_FILE;

   SearchAndRescue(Mutex *mtx);

   virtual ~SearchAndRescue();

   virtual SearchAndRescue *runConstructor(ObjList &stack, const ExpOperation &oper, GenObject *context);

   static void initialize(ScriptContext *context);

protected:

   // bool runNative(ObjList &stack, const ExpOperation &oper, GenObject *context);

   bool runFunction(ObjList &stack, const ExpOperation &oper, GenObject *context);

   void write_to_ocp(ObjList &stack, const ExpOperation &oper, GenObject *context);

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
   params().addParam(new ExpFunction("writePhyinfo"));
   params().addParam(new ExpFunction("writeSms"));
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

bool SearchAndRescue::runFunction(ObjList &stack, const ExpOperation &oper, GenObject *context) {
   if (oper.name() == YSTRING("writeToOCP"))
      this->write_to_ocp(stack, oper, context);
   return true;
}

void SearchAndRescue::write_to_ocp(ObjList &stack, const ExpOperation &oper, GenObject *context) {
   ObjList args;
   int argc = extractArgs(stack, oper, context, args);
   String tmp;
   tmp << "SearchAndRescue::write_to_ocp '";
   std::string message;

   // Evaluate
   if (argc < 1) {
      tmp << " ''";
      ExpEvaluator::pushOne(stack, new ExpOperation(tmp));      
   } else {
      ExpOperation *msg = static_cast<ExpOperation*>(args[0]);
      tmp << " '" << msg->c_str() << "'";
      ExpEvaluator::pushOne(stack, new ExpOperation(tmp));
      message = std::string(msg->c_str());
   }

   // write to the POSIX queue
   MQCommon::push((message + "\n").c_str());

   // write to the log file
   std::ofstream sar_log;
   sar_log.open(SearchAndRescue::OUT_FILE, std::ios_base::app);
   sar_log << message.c_str() << std::endl;
   sar_log.close();
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

