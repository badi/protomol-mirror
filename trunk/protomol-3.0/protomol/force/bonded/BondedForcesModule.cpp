#include "BondedForcesModule.h"

#include "DihedralSystemForce.h"
#include "BondSystemForce.h"
#include "AngleSystemForce.h"
#include "ImproperSystemForce.h"
#include "HarmDihedralSystemForce.h"

#include <protomol/base/ProtoMolApp.h>
#include <protomol/base/StringUtilities.h>
#include <protomol/topology/TopologyModule.h>
#include <protomol/topology/PeriodicBoundaryConditions.h>
#include <protomol/topology/VacuumBoundaryConditions.h>

using namespace std;
using namespace ProtoMol;

void BondedForcesModule::init(ProtoMolApp *app) {}

void BondedForcesModule::registerForces(ProtoMolApp *app) {
  ForceFactory &f = app->forceFactory;

  string boundConds =
    app->config[InputBoundaryConditions::keyword];

  if (equalNocase(boundConds, PeriodicBoundaryConditions::keyword)) {
    f.registerExemplar(new DihedralSystemForce<PeriodicBoundaryConditions>());
    f.registerExemplar(new BondSystemForce<PeriodicBoundaryConditions>());
    f.registerExemplar(new AngleSystemForce<PeriodicBoundaryConditions>());
    f.registerExemplar(new ImproperSystemForce<PeriodicBoundaryConditions>());
    f.registerExemplar(new
                       HarmDihedralSystemForce<PeriodicBoundaryConditions>());

  } else if (equalNocase(boundConds, VacuumBoundaryConditions::keyword)) {
    f.registerExemplar(new DihedralSystemForce<VacuumBoundaryConditions>());
    f.registerExemplar(new BondSystemForce<VacuumBoundaryConditions>());
    f.registerExemplar(new AngleSystemForce<VacuumBoundaryConditions>());
    f.registerExemplar(new ImproperSystemForce<VacuumBoundaryConditions>());
    f.registerExemplar(new HarmDihedralSystemForce<VacuumBoundaryConditions>());
  }
}
