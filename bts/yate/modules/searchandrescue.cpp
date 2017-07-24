#include <yatengine.h>
#include <yatescript.h>

#include <fstream>
#include <limits>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <ctime>

using namespace TelEngine;

struct Phyinfo {

   float TA;
   float TE;
   float UpRSSI;
   float TxPwr;
   float DnRSSIdBm;
   float time;

   Phyinfo();

};

struct SMS {
   
   std::string message;

   SMS();
};

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

   void write_packet(std::string imsi, std::string tmsi, std::string type, std::string data_json);

   void write_phyinfo(ObjList &stack, const ExpOperation &oper, GenObject *context);

   void write_sms(ObjList &stack, const ExpOperation &oper, GenObject *context);

   std::string build_json(const Phyinfo info);

   std::string build_json(const SMS sms);

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

// --- Phyinfo ---

Phyinfo::Phyinfo() : 
      TA(std::numeric_limits<float>::min()),
      TE(std::numeric_limits<float>::min()),
      UpRSSI(std::numeric_limits<float>::min()), 
      TxPwr(std::numeric_limits<float>::min()), 
      DnRSSIdBm(std::numeric_limits<float>::min()), 
      time(std::numeric_limits<float>::min()) { }

// --- SMS ---

SMS::SMS() : message("") { }

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

bool SearchAndRescue::runNative(ObjList &stack, const ExpOperation &oper, GenObject *context) {
   if (oper.name() == YSTRING("writePhyinfo")) {
      this->write_phyinfo(stack, oper, context);
   } else if (oper.name() == YSTRING("writeSms")) {
      this->write_sms(stack, oper, context);
   } else {
      return JsObject::runNative(stack, oper, context);
   }
   return true;
}

void SearchAndRescue::write_packet(std::string imsi, std::string tmsi, std::string type, std::string data_json) {
   std::ofstream file;
   file.open(SearchAndRescue::OUT_FILE, std::ios_base::app);
   // file.open(SearchAndRescue::OUT_FILE, std::ios_base::trunc);

   long int timestamp = time(0); // now

   file << "{\"IMSI\": \"" << imsi << "\", \"TMSI\": \"" << tmsi << "\", \"type\": \"" 
      << type << "\", \"timestamp\": \"" << timestamp << "\", \"data\": " 
      << data_json << "}" << std::endl;

   file.close();
}

void SearchAndRescue::write_phyinfo(ObjList &stack, const ExpOperation &oper, GenObject *context) {

   ObjList args;
   int argc = extractArgs(stack, oper, context, args);
   Phyinfo phyinfo;
   std::string imsi;
   std::string tmsi;
   String tmp;
   tmp << "SearchAndRescue: argc=" << argc;

   for (int i = 0; i < argc; ++i) {
      ExpOperation *op = static_cast<ExpOperation*>(args[i]);
      if (!op || !op->c_str()) continue;

      tmp << " '" << *op << "'";

      switch (i) {
      case 0:
         imsi = std::string(op->c_str());
         break;
      case 1:
         tmsi = std::string(op->c_str());
         break;
      case 2:
         phyinfo.TA = atof(op->c_str());
         break;
      case 3:
         phyinfo.TE = atof(op->c_str());
         break;
      case 4:
         phyinfo.UpRSSI = atof(op->c_str());
         break;
      case 5:
         phyinfo.TxPwr = atof(op->c_str());
         break;
      case 6:
         phyinfo.DnRSSIdBm = atof(op->c_str());
         break;
      case 7:
         phyinfo.time = atof(op->c_str());
         break;
      }
   }

   ExpEvaluator::pushOne(stack, new ExpOperation(tmp));

   this->write_packet(imsi, tmsi, std::string("phy"), this->build_json(phyinfo));
}

void SearchAndRescue::write_sms(ObjList &stack, const ExpOperation &oper, GenObject *context) {

   ObjList args;
   int argc = extractArgs(stack, oper, context, args);
   SMS sms;
   std::string imsi;
   std::string tmsi;
   String tmp;
   tmp << "SearchAndRescue: argc=" << argc;

   for (int i = 0; i < argc; ++i) {
      ExpOperation *op = static_cast<ExpOperation*>(args[i]);
      tmp << " '" << *op << "'";

      switch (i) {
      case 0:
         imsi = std::string(op->c_str());
         break;
      case 1:
         tmsi = std::string(op->c_str());
         break;
      case 2:
         sms.message = std::string(op->c_str());
         break;
      }
   }

   ExpEvaluator::pushOne(stack, new ExpOperation(tmp));

   this->write_packet(imsi, tmsi, std::string("sms"), this->build_json(sms));
}

std::string SearchAndRescue::build_json(const Phyinfo info) {
   std::stringstream result;

   result << "{";

   result << "\"TA\": " << info.TA << ", ";
   result << "\"TE\": " << info.TE << ", ";
   result << "\"UpRSSI\": " << info.UpRSSI << ", ";
   result << "\"TxPwr\": " << info.TxPwr << ", ";
   result << "\"DnRSSIdBm\": " << info.DnRSSIdBm << ", ";
   result << "\"time\": " << info.time;

   result << "}";

   return result.str();
}

std::string SearchAndRescue::build_json(const SMS sms) {
   std::stringstream result;

   result << "{";

   result << "\"message\": \"" << sms.message;

   result << "\"}";

   return result.str();
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

