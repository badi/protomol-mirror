#include "LangevinImpulseModule.h"
#include "LangevinImpulseIntegrator.h"

#include <protomol/frontend/ProtoMolApp.h>

using namespace std;
using namespace ProtoMol;

void LangevinImpulseModule::init(ProtoMolApp *app) {
  app->integratorFactory.registerExemplar(new LangevinImpulseIntegrator());
}
