#include <protomol/integrator/normal/NormalModeModule.h>

#include <protomol/base/ProtoMolApp.h>

#include <protomol/integrator/normal/NormalModeLangevin.h>
#include <protomol/integrator/normal/NormalModeMinimizer.h>
#include <protomol/integrator/normal/NormalModeDiagonalize.h>
#include <protomol/integrator/normal/NormalModeMori.h>
#include <protomol/integrator/normal/NormalModeRelax.h>
#include <protomol/integrator/normal/NormalModeBrownian.h>

using namespace std;
using namespace ProtoMol;

void NormalModeModule::init(ProtoMolApp *app) {
  app->integratorFactory.registerExemplar(new NormalModeLangevin());
  app->integratorFactory.registerExemplar(new NormalModeMinimizer());
  app->integratorFactory.registerExemplar(new NormalModeDiagonalize());
  app->integratorFactory.registerExemplar(new NormalModeMori());
  app->integratorFactory.registerExemplar(new NormalModeRelax());
  app->integratorFactory.registerExemplar(new NormalModeBrownian());
}
