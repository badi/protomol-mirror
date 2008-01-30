#include <protomol/integrator/STSIntegrator.h>
#include <protomol/util/Report.h>
#include <protomol/types/ScalarStructure.h>
#include <protomol/types/Vector3DBlock.h>
#include <protomol/force/ForceGroup.h>
#include <protomol/topology/GenericTopology.h>
#include <protomol/topology/TopologyUtilities.h>
#include <protomol/util/PMConstants.h>
#include <protomol/modifier/ModifierIncrementTimestep.h>

using namespace ProtoMol::Report;
using std::string;
using std::vector;

namespace ProtoMol {
  //________________________________________ STSIntegrator

  STSIntegrator::STSIntegrator() : StandardIntegrator(), myTimestep(0.0) {}

  STSIntegrator::STSIntegrator(Real timestep, ForceGroup *overloadedForces) :
    StandardIntegrator(overloadedForces), myTimestep(timestep) {}

  void STSIntegrator::initialize(GenericTopology *topo,
                                 Vector3DBlock *positions,
                                 Vector3DBlock *velocities,
                                 ScalarStructure *energies) {
    StandardIntegrator::initialize(topo, positions, velocities, energies);
  }

  void STSIntegrator::addModifierAfterInitialize() {
    //Report::report <<"[STSIntegrator::addModifierAfterInitialize]"<<Report::endr;
    adoptPostForceModifier(new ModifierIncrementTimestep(this));
  }

  void STSIntegrator::doDriftOrNextIntegrator() {
    preDriftOrNextModify();
    doDrift();
    postDriftOrNextModify();
  }

  void STSIntegrator::calculateForces() {
    //  -------------------------------------------------------------------  //
    //  Energies have to be cleared for the innermost integrator which is    //
    //  always an STS integrator.  Potential energy is U^{short}(X1) +       //
    //  U^{long}(X1) + ... +U^{last}(X1) forces are gradient of potential,   //
    //  thus they are set to zero here also.                                 //
    //                                                                       //
    //  Forces are cleared in StandardIntegrator::calculateForces()          //
    //  -------------------------------------------------------------------  //

    myEnergies->clear();
    StandardIntegrator::calculateForces();
  }

  void STSIntegrator::doDrift() {
    Real h = getTimestep() * Constant::INV_TIMEFACTOR;
    myPositions->intoWeightedAdd(h, *myVelocities);
    buildMolecularCenterOfMass(myPositions, myTopo);
  }

  void STSIntegrator::getParameters(std::vector<Parameter> &parameter) const {
    parameter.push_back(Parameter("timestep",
        Value(myTimestep, ConstraintValueType::Positive())));
  }

  STSIntegrator *STSIntegrator::make(string &errMsg,
                                     const vector<Value> &values,
                                     ForceGroup *fg) const {
    errMsg = "";
    if (!checkParameters(errMsg, values))
      return NULL;
    return adjustAlias(doMake(errMsg, values, fg));
  }
}
