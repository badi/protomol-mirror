#include "NonbondedSimpleFullForceModule.h"

#include <protomol/base/ProtoMolApp.h>
#include <protomol/base/StringUtilities.h>
#include <protomol/topology/TopologyModule.h>
#include <protomol/topology/PeriodicBoundaryConditions.h>
#include <protomol/topology/VacuumBoundaryConditions.h>

#include <protomol/force/OneAtomPair.h>
#include <protomol/force/OneAtomPairTwo.h>
#include <protomol/force/CoulombForce.h>
#include <protomol/force/LennardJonesForce.h>
#include <protomol/force/coulomb/CoulombSCPISMForce.h>
#include <protomol/force/coulomb/CoulombBornRadiiForce.h>
#include <protomol/force/coulomb/CoulombForceDiElec.h>

#include <protomol/switch/C1SwitchingFunction.h>
#include <protomol/switch/C2SwitchingFunction.h>
#include <protomol/switch/CmpCnCnSwitchingFunction.h>
#include <protomol/switch/CnSwitchingFunction.h>
#include <protomol/switch/ComplementSwitchingFunction.h>

#include <protomol/force/nonbonded/NonbondedSimpleFullSystemForce.h>

#include <protomol/topology/CellListEnumeratorPeriodicBoundaries.h>
#include <protomol/topology/CellListEnumeratorStandard.h>

using namespace std;
using namespace ProtoMol;

void NonbondedSimpleFullForceModule::registerForces(ProtoMolApp *app) {
  ForceFactory &f = app->forceFactory;
  string boundConds = app->config[InputBoundaryConditions::keyword];

  typedef PeriodicBoundaryConditions PBC;
  typedef VacuumBoundaryConditions VBC;
  typedef C1SwitchingFunction C1;
  typedef C2SwitchingFunction C2;
  typedef CnSwitchingFunction Cn;
  typedef CmpCnCnSwitchingFunction CmpCnCn;
#define Complement ComplementSwitchingFunction 
#define SimpleFullSystem NonbondedSimpleFullSystemForce

  if (equalNocase(boundConds, PeriodicBoundaryConditions::keyword)) {

    // NonbondedSimpleFullSystemForce CoulombForce
    f.reg(new SimpleFullSystem<OneAtomPair<PBC, C1, CoulombForce> >());
    f.reg(new SimpleFullSystem<OneAtomPair<PBC, Complement<C1>,
          CoulombForce> >());
    f.reg(new SimpleFullSystem<OneAtomPair<PBC, Complement<C2>,
          CoulombForce> >());
    f.reg(new SimpleFullSystem<OneAtomPair<PBC, Complement<Cn>,
          CoulombForce> >());
    
    // NonbondedSimpleFullSystemForce LennardJonesForce
    f.reg(new SimpleFullSystem<OneAtomPair<PBC, C2, LennardJonesForce> >());
    f.reg(new SimpleFullSystem<OneAtomPair<PBC, Cn, LennardJonesForce> >());
    f.reg(new SimpleFullSystem<OneAtomPair<PBC, Complement<C1>,
          LennardJonesForce> >());
    f.reg(new SimpleFullSystem<OneAtomPair<PBC, Complement<C2>,
          LennardJonesForce> >());
    f.reg(new SimpleFullSystem<OneAtomPair<PBC, Complement<Cn>,
          LennardJonesForce> >());
    
    // NonbondedSimpleFullSystemForce LennardJonesForce CoulombForce
    f.reg(new SimpleFullSystem<OneAtomPairTwo<PBC, Complement<C2>,
          LennardJonesForce, Complement<C1>, CoulombForce> >());
    f.reg(new SimpleFullSystem<OneAtomPairTwo<PBC, Complement<Cn>,
          LennardJonesForce, Complement<C1>, CoulombForce> >());

  } else if (equalNocase(boundConds,  VacuumBoundaryConditions::keyword)) {

    // NonbondedSimpleFullSystemForce CoulombForce
    f.reg(new SimpleFullSystem<OneAtomPair<VBC, C1, CoulombForce> >());
    f.reg(new SimpleFullSystem<OneAtomPair<VBC, Complement<C1>,
          CoulombForce> >());
    f.reg(new SimpleFullSystem<OneAtomPair<VBC, Complement<C2>,
          CoulombForce> >());
    f.reg(new SimpleFullSystem<OneAtomPair<VBC, Complement<Cn>,
          CoulombForce> >());

    // NonbondedSimpleFullSystemForce CoulombForceDiElec
    f.reg(new SimpleFullSystem<OneAtomPair<VBC, C1, CoulombForceDiElec> >());
    f.reg(new SimpleFullSystem<OneAtomPair<VBC, Complement<C1>,
          CoulombForceDiElec> >());
    f.reg(new SimpleFullSystem<OneAtomPair<VBC, Complement<C2>,
          CoulombForceDiElec> >());
    f.reg(new SimpleFullSystem<OneAtomPair<VBC, Complement<Cn>,
          CoulombForceDiElec> >());

    // NonbondedSimpleFullSystemForce LennardJonesForce
    f.reg(new SimpleFullSystem<OneAtomPair<VBC, C2, LennardJonesForce> >());
    f.reg(new SimpleFullSystem<OneAtomPair<VBC, Cn, LennardJonesForce> >());
    f.reg(new SimpleFullSystem<OneAtomPair<VBC, Complement<C1>,
          LennardJonesForce> >());
    f.reg(new SimpleFullSystem<OneAtomPair<VBC, Complement<C2>,
          LennardJonesForce> >());
    f.reg(new SimpleFullSystem<OneAtomPair<VBC, Complement<Cn>,
          LennardJonesForce> >());

    // NonbondedSimpleFullSystemForce LennardJonesForce CoulombForce
    f.reg(new SimpleFullSystem<OneAtomPairTwo<VBC, Complement<C2>,
          LennardJonesForce, Complement<C1>, CoulombForce> >());
    f.reg(new SimpleFullSystem<OneAtomPairTwo<VBC, Complement<Cn>,
          LennardJonesForce, Complement<C1>, CoulombForce> >());

    // NonbondedSimpleFullSystemForce LennardJonesForce CoulombForceDiElec
    f.reg(new SimpleFullSystem<OneAtomPairTwo<VBC, Complement<C2>,
          LennardJonesForce, Complement<C1>, CoulombForceDiElec> >());
    f.reg(new SimpleFullSystem<OneAtomPairTwo<VBC, Complement<Cn>,
          LennardJonesForce, Complement<C1>, CoulombForceDiElec> >());
  }
}
