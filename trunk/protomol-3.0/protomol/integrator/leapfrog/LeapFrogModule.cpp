#include "LeapfrogModule.h"

#include <protomol/base/ProtoMolApp.h>

#include "LeapfrogIntegrator.h"
#include "LeapfrogTruncatedShadow.h"
#include "DMDLeapfrogIntegrator.h"
#include "PLeapfrogIntegrator.h"
#include "NoseNVTLeapfrogIntegrator.h"

using namespace std;
using namespace ProtoMol;

void LeapfrogModule::init(ProtoMolApp *app) {
  app->integratorFactory.registerExemplar(new LeapfrogIntegrator());
  app->integratorFactory.registerExemplar(new LeapfrogTruncatedShadow());
  app->integratorFactory.registerExemplar(new DMDLeapfrogIntegrator());
  app->integratorFactory.registerExemplar(new PLeapfrogIntegrator());
  app->integratorFactory.registerExemplar(new NoseNVTLeapfrogIntegrator());
}
