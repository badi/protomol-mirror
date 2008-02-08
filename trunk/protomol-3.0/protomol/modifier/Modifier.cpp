#include "Modifier.h"

#include <protomol/base/ProtoMolApp.h>
#include <protomol/base/Report.h>
#include <protomol/topology/GenericTopology.h>

#include <sstream>

using namespace std;
using namespace ProtoMol::Report;
using namespace ProtoMol;

void Modifier::execute() {
  stringstream str;
  print(str);
  
  report
    << debug(10) << "[Modifier::execute] " << str.str()
    << "(" << (long)(this) << ") (enable=" << myEnable << ") at "
    << app->topology->time << endr;
  
  if (myEnable) doExecute();
}

void Modifier::initialize(ProtoMolApp *app, Vector3DBlock *forces) {
  this->app = app;
  myForces = forces;
  
  stringstream str;
  print(str);
  report << debug(10) << "[Modifier::initialize] " << str.str() << "("
         << (long)(this) << ") at " << app->topology->time << endr;
  
  doInitialize();
}
