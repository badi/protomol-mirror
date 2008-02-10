#include "NormalModeModule.h"

#include <protomol/base/ProtoMolApp.h>

#include "NormalModeLangevin.h"
#include "NormalModeMinimizer.h"
#include "NormalModeDiagonalize.h"
#include "NormalModeMori.h"
#include "NormalModeRelax.h"
#include "NormalModeBrownian.h"

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
