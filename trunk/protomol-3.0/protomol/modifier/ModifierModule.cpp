#include "ModifierModule.h"

#include <protomol/modifier/ModifierIncrementTimestep.h>
#include <protomol/modifier/ModifierRattle.h>
#include <protomol/modifier/ModifierShake.h>
#include <protomol/modifier/ModifierRemoveAngularMomentum.h>
#include <protomol/modifier/ModifierRemoveLinearMomentum.h>

#include <protomol/base/Report.h>
#include <protomol/base/ProtoMolApp.h>
#include <protomol/topology/TopologyUtilities.h>

using namespace std;
using namespace ProtoMol::Report;
using namespace ProtoMol;

defineInputValueWithAliasesAndText
(InputRemoveLinearMomentum, "removeLinearMomentum", ("comMotion"),
  "removes linear momentum, where -1 for never, 0 at initialization or at STS "
  "frequency <n>");
defineInputValueWithAliasesAndText
(InputRemoveAngularMomentum, "removeAngularMomentum", ("angularMomentum"),
  "removes angular momentum, where -1 for never, 0 at initialization or at STS "
  "frequency <n>");

void ModifierModule::init(ProtoMolApp *app) {
  //app->modifierFactory.registerExemplar();

  InputRemoveLinearMomentum::registerConfiguration(&app->config);
  InputRemoveAngularMomentum::registerConfiguration(&app->config);
}

void ModifierModule::postBuild(ProtoMolApp *app) {
    // Remove Linear Momentum
  if ((int)app->config[InputRemoveLinearMomentum::keyword] >= 0)
    report
      << plain << "Removed linear momentum: "
      << removeLinearMomentum(&app->velocities, app->topology) *
      Constant::INV_TIMEFACTOR << endr;

  else
    report
      << plain << "Linear momentum : "
      << linearMomentum(&app->velocities, app->topology) *
      Constant::INV_TIMEFACTOR << endr;

  // Remove Angular Momentum
  if ((int)app->config[InputRemoveAngularMomentum::keyword] >= 0)
    report
      << plain << "Removed angular momentum : "
      << removeAngularMomentum(&app->positions, &app->velocities,
                               app->topology) *
      Constant::INV_TIMEFACTOR << endr;
  
  else
    report
      << plain << "Angular momentum : "
      << angularMomentum(&app->positions, &app->velocities, app->topology) *
      Constant::INV_TIMEFACTOR << endr;
}
