#include <protomol/integrator/StandardIntegrator.h>
#include <protomol/util/Report.h>
#include <protomol/types/ScalarStructure.h>
#include <protomol/types/Vector3DBlock.h>
#include <protomol/force/ForceGroup.h>
#include <protomol/topology/GenericTopology.h>
#include <protomol/topology/TopologyUtilities.h>
#include <protomol/util/PMConstants.h>

#ifdef HAVE_PARALLE
#include "Parallel.h"
#endif // HAVE_PARALLE

using namespace ProtoMol;
//____ StandardIntegrator
StandardIntegrator::StandardIntegrator() :
  Integrator(), myPreviousIntegrator(NULL) {}

StandardIntegrator::StandardIntegrator(ForceGroup *forceGroup) :
  Integrator(forceGroup), myPreviousIntegrator(NULL) {}

void StandardIntegrator::run(int numTimesteps) {
  for (int i = 0; i < numTimesteps; i++) {
    preStepModify();
    doHalfKick();
    doDriftOrNextIntegrator();
    calculateForces();
    doHalfKick();
    postStepModify();
  }
}

void StandardIntegrator::initialize(GenericTopology *topo,
                                    Vector3DBlock   *positions,
                                    Vector3DBlock   *velocities,
                                    ScalarStructure *energies) {
  Integrator::initialize(topo, positions, velocities, energies);
}

void StandardIntegrator::initializeForces() {
  addModifierBeforeInitialize();
  calculateForces();
  addModifierAfterInitialize();
}

void StandardIntegrator::calculateForces() {
  //  Save current value of potentialEnergy().
  myPotEnergy = myEnergies->potentialEnergy();

  myForces->zero();
  preForceModify();

#ifdef HAVE_PARALLEL
  if (!anyMediForceModify())
    Parallel::distribute(myEnergies, myForces);
#endif // HAVE_PARALLEL

  myForcesToEvaluate->
    evaluateSystemForces(myTopo, myPositions, myForces, myEnergies);
  mediForceModify();
  myForcesToEvaluate->
    evaluateExtendedForces(myTopo, myPositions, myVelocities, myForces,
    myEnergies);

#ifdef HAVE_PARALLEL
  if (!anyMediForceModify())
    Parallel::reduce(myEnergies, myForces);
#endif // HAVE_PARALLEL

  postForceModify();

  //  Compute my potentialEnergy as the difference before/after the call to
  //  calculateForces().
  myPotEnergy = myEnergies->potentialEnergy() - myPotEnergy;
}

void StandardIntegrator::doHalfKick() {
  Real h = 0.5 * getTimestep() * Constant::INV_TIMEFACTOR;
  const unsigned int count = myPositions->size();

  updateBeta(h);

  for (unsigned int i = 0; i < count; ++i)
    (*myVelocities)[i] += (*myForces)[i] * h / myTopo->atoms[i].scaledMass;

  buildMolecularMomentum(myVelocities, myTopo);
}

void StandardIntegrator::doKick() {
  Real h = getTimestep() * Constant::INV_TIMEFACTOR;
  const unsigned int count = myPositions->size();

  updateBeta(h);

  for (unsigned int i = 0; i < count; ++i)
    (*myVelocities)[i] += (*myForces)[i] * h / myTopo->atoms[i].scaledMass;

  buildMolecularMomentum(myVelocities, myTopo);
}

Integrator *StandardIntegrator::previous() {
  return myPreviousIntegrator;
}

const Integrator *StandardIntegrator::previous() const {
  return myPreviousIntegrator;
}

