#include "IntegratorBaseModule.h"

#include "LangevinImpulseIntegrator.h"

#include <protomol/base/ProtoMolApp.h>

using namespace std;
using namespace ProtoMol;

void IntegratorBaseModule::init(ProtoMolApp *app) {
  app->integratorFactory.registerExemplar(new LangevinImpulseIntegrator());
}
