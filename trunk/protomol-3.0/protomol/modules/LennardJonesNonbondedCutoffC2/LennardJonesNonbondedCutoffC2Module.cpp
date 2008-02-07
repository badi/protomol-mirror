#include <protomol/modules/LennardJonesNonbondedCutoffC2/LennardJonesNonbondedCutoffC2Module.h>

#include <protomol/switch/C2SwitchingFunction.h>
#include <protomol/modules/LennardJonesNonbondedCutoffC2/NonbondedCutoffSystemForce.h>
#include <protomol/force/LennardJonesForce.h>
#include <protomol/force/OneAtomPair.h>
#include <protomol/topology/CubicCellManager.h>
#include <protomol/topology/CellListEnumeratorStandard.h>
#include <protomol/topology/CellListEnumeratorPeriodicBoundaries.h>

#include <protomol/base/ProtoMolApp.h>
#include <protomol/base/StringUtilities.h>
#include <protomol/topology/TopologyModule.h>
#include <protomol/topology/PeriodicBoundaryConditions.h>
#include <protomol/topology/VacuumBoundaryConditions.h>

using namespace std;
using namespace ProtoMol;

void LennardJonesNonbondedCutoffC2Module::registerForces(ProtoMolApp *app) {
  ForceFactory &f = app->forceFactory;

  string boundConds =
    app->config[InputBoundaryConditions::keyword];

  if (equalNocase(boundConds, PeriodicBoundaryConditions::keyword)) {
    f.registerExemplar(new NonbondedCutoffSystemForce<CubicCellManager,
                       OneAtomPair<PeriodicBoundaryConditions,
                       C2SwitchingFunction, LennardJonesForce> >());

  } else if (equalNocase(boundConds, VacuumBoundaryConditions::keyword)) {
    f.registerExemplar(new NonbondedCutoffSystemForce<CubicCellManager,
                       OneAtomPair<VacuumBoundaryConditions,
                       C2SwitchingFunction, LennardJonesForce> >());
  }
}

