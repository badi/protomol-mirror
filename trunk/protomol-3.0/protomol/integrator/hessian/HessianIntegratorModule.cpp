#include "HessianIntegratorModule.h"
#include "HessianInt.h"

#include <protomol/base/ProtoMolApp.h>

using namespace std;
using namespace ProtoMol;

void HessianIntegratorModule::init(ProtoMolApp *app) {
  app->integratorFactory.registerExemplar(new HessianInt());
}
