#include <protomol/integrator/Integrator.h>
#include <protomol/util/Report.h>
#include <protomol/types/ScalarStructure.h>
#include <protomol/types/Vector3DBlock.h>
#include <protomol/force/ForceGroup.h>
#include <protomol/topology/GenericTopology.h>
#include <protomol/topology/TopologyUtilities.h>
#include <protomol/modifier/Modifier.h>
#include <protomol/debug/Zap.h>

using namespace std;
using namespace ProtoMol;
using namespace ProtoMol::Report;

//____ Integrator

//____  Initialize static members.
const string Integrator::scope("Integrator");
Real Integrator::myBeta = 0.;

Integrator::Integrator() :
  Makeable(), myPotEnergy(0), mhQ(0), myTopo(0), myPositions(0),
  myVelocities(0), myForces(0), myEnergies(0), myForcesToEvaluate(0),
  myForward(true), myOldForces(0) {}

Integrator::Integrator(ForceGroup *forceGroup) :
  Makeable(), myPotEnergy(0), mhQ(0), myTopo(0), myPositions(0),
  myVelocities(0), myForces(new Vector3DBlock), myEnergies(0),
  myForcesToEvaluate(forceGroup), myForward(true),
  myOldForces(new Vector3DBlock) {}

Integrator::~Integrator() {
  zap(myForces);
  zap(myOldForces);
  zap(myForcesToEvaluate);

  for (set<Modifier *>::iterator i = myListModifiers.begin();
       i != myListModifiers.end(); ++i)
    delete (*i);
}

void Integrator::initialize(GenericTopology *topo, Vector3DBlock *positions,
                            Vector3DBlock   *velocities,
                            ScalarStructure *energies) {
  myTopo = topo;
  myPositions = positions;
  myVelocities = velocities;
  myEnergies = energies;

  myForces->zero(positions->size());
  myOldForces->zero(positions->size());

  buildMolecularCenterOfMass(myPositions, myTopo);
  buildMolecularMomentum(myVelocities, myTopo);

  // Initialize only external modifiers,
  // where internal modifiers will be added
  // and initialized at appropriated time
  deleteInternalModifiers();
  initializeModifiers();
}

Integrator *Integrator::top() {
  Integrator *i = this;
  for (; i->previous() != 0; i = i->previous()) ;

  return i;
}

const Integrator *Integrator::top() const {
  const Integrator *i = this;
  for (; i->previous() != 0; i = i->previous()) ;

  return i;
}

Integrator *Integrator::bottom() {
  Integrator *i = this;
  for (; i->next() != 0; i = i->next()) ;

  return i;
}

const Integrator *Integrator::bottom() const {
  const Integrator *i = this;
  for (; i->next() != 0; i = i->next()) ;

  return i;
}

int Integrator::level() const {
  int n = 0;
  for (const Integrator *i = this; i->next() != 0; i = i->next())
    n++;

  return n;
}

IntegratorDefinition Integrator::getIntegratorDefinition() const {
  IntegratorDefinition tmp;

  // Integrator definition
  tmp.integrator.id = this->getId();
  this->getParameters(tmp.integrator.parameters);

  // Force definitions
  if (myForcesToEvaluate != 0)
    myForcesToEvaluate->getDefinition(tmp.forces);

  return tmp;
}

vector<IntegratorDefinition> Integrator::getIntegratorDefinitionAll() const {
  vector<IntegratorDefinition> res;
  for (const Integrator *i = bottom(); i != 0; i = i->previous())
    res.push_back(i->getIntegratorDefinition());

  return res;
}

void Integrator::uncache() {
  for (Integrator *i = top(); i != 0; i = i->next()) {
    if (i->myForcesToEvaluate != 0)
      i->myForcesToEvaluate->uncache();
    i->doUncache();
  }
}

void Integrator::forward() {
  for (Integrator *i = top(); i != 0; i = i->next())
    i->myForward = true;
}

void Integrator::backward() {
  for (Integrator *i = top(); i != 0; i = i->next())
    i->myForward = false;
}

void Integrator::preStepModify() {
  report << debug(10) << "[Integrator::preStepModify] (" <<
  (long)this << ") " << (myTopo != 0 ? myTopo->time : 0.0) << endr;
  for (set<Modifier *>::iterator i = myPreStepModifiers.begin();
       i != myPreStepModifiers.end();
       ++i)
    (*i)->execute();
}

void Integrator::preDriftOrNextModify() {
  report << debug(10) <<
  "[Integrator::preDriftOrNextModify] (" << (long)this << ") " <<
  (myTopo != 0 ? myTopo->time : 0.0) << endr;
  for (set<Modifier *>::iterator i = myPreDriftOrNextModifiers.begin();
       i != myPreDriftOrNextModifiers.end();
       ++i)
    (*i)->execute();
}

void Integrator::postDriftOrNextModify() {
  report << debug(10) <<
  "[Integrator::postDriftOrNextModify] (" << (long)this << ") " <<
  (myTopo != 0 ? myTopo->time : 0.0) << endr;
  for (set<Modifier *>::iterator i = myPostDriftOrNextModifiers.begin();
       i != myPostDriftOrNextModifiers.end();
       ++i)
    (*i)->execute();
}

void Integrator::preForceModify() {
  report << debug(10) << "[Integrator::preForceModify] (" <<
  (long)this << ") " << (myTopo != 0 ? myTopo->time : 0.0) << endr;
  for (set<Modifier *>::iterator i = myPreForceModifiers.begin();
       i != myPreForceModifiers.end();
       ++i)
    (*i)->execute();
}

void Integrator::mediForceModify() {
  report << debug(10) << "[Integrator::mediForceModify] (" <<
  (long)this << ") " << (myTopo != 0 ? myTopo->time : 0.0) << endr;
  for (set<Modifier *>::iterator i = myMediForceModifiers.begin();
       i != myMediForceModifiers.end();
       ++i)
    (*i)->execute();
}

void Integrator::postForceModify() {
  report << debug(10) << "[Integrator::postForceModify] (" <<
  (long)this << ") " << (myTopo != 0 ? myTopo->time : 0.0) << endr;
  for (set<Modifier *>::iterator i = myPostForceModifiers.begin();
       i != myPostForceModifiers.end();
       ++i)
    (*i)->execute();
}

void Integrator::postStepModify() {
  report << debug(10) << "[Integrator::postStepModify] (" <<
  (long)this << ") " << (myTopo != 0 ? myTopo->time : 0.0) << endr;
  for (set<Modifier *>::iterator i = myPostStepModifiers.begin();
       i != myPostStepModifiers.end();
       ++i)
    (*i)->execute();
}

//____  ---------------------------------------------------------------------  //

void Integrator::adoptPreStepModifier(Modifier *modifier) {
  report << debug(10) <<
  "[Integrator::adoptPreStepModifier] " << modifier->print() << "(" <<
  (long)modifier << ") " <<
  (myTopo != 0 ? myTopo->time : 0.0) << endr;
  if (myTopo != 0)
    modifier->initialize(myTopo,
      myPositions,
      myVelocities,
      myForces,
      myEnergies);
  myPreStepModifiers.insert(modifier);
  addModifier(modifier);
}

void Integrator::adoptPreDriftOrNextModifier(Modifier *modifier) {
  report << debug(10) <<
  "[Integrator::adoptPreDriftOrNextModifier] " << modifier->print() << "(" <<
  (long)modifier << ") " <<
  (myTopo != 0 ? myTopo->time : 0.0) << endr;
  if (myTopo != 0)
    modifier->initialize(myTopo,
      myPositions,
      myVelocities,
      myForces,
      myEnergies);
  myPreDriftOrNextModifiers.insert(modifier);
  addModifier(modifier);
}

void Integrator::adoptPostDriftOrNextModifier(Modifier *modifier) {
  report << debug(10) <<
  "[Integrator::adoptPostDriftOrNextModifier] " << modifier->print() <<
  "(" <<
  (long)modifier << ") " <<
  (myTopo != 0 ? myTopo->time : 0.0) << endr;
  if (myTopo != 0)
    modifier->initialize(myTopo,
      myPositions,
      myVelocities,
      myForces,
      myEnergies);
  myPostDriftOrNextModifiers.insert(modifier);
  addModifier(modifier);
}

void Integrator::adoptPreForceModifier(Modifier *modifier) {
  report << debug(10) <<
  "[Integrator::adoptPreForceModifier] " << modifier->print() << "(" <<
  (long)modifier << ") " <<
  (myTopo != 0 ? myTopo->time : 0.0) << endr;
  if (myTopo != 0)
    modifier->initialize(myTopo,
      myPositions,
      myVelocities,
      myForces,
      myEnergies);
  myPreForceModifiers.insert(modifier);
  addModifier(modifier);
}

void Integrator::adoptMediForceModifier(Modifier *modifier) {
  report << debug(10) <<
  "[Integrator::adoptMediForceModifier] " << modifier->print() << "(" <<
  (long)modifier << ") " <<
  (myTopo != 0 ? myTopo->time : 0.0) << endr;
  if (myTopo != 0)
    modifier->initialize(myTopo,
      myPositions,
      myVelocities,
      myForces,
      myEnergies);
  myMediForceModifiers.insert(modifier);
  addModifier(modifier);
}

void Integrator::adoptPostForceModifier(Modifier *modifier) {
  report << debug(10) <<
  "[Integrator::adoptPostForceModifier] " << modifier->print() << "(" <<
  (long)modifier << ") " <<
  (myTopo != 0 ? myTopo->time : 0.0) << endr;
  if (myTopo != 0)
    modifier->initialize(myTopo,
      myPositions,
      myVelocities,
      myForces,
      myEnergies);
  myPostForceModifiers.insert(modifier);
  addModifier(modifier);
}

void Integrator::adoptPostStepModifier(Modifier *modifier) {
  report << debug(10) <<
  "[Integrator::adoptPostStepModifier] " << modifier->print() << "(" <<
  (long)modifier << ") " <<
  (myTopo != 0 ? myTopo->time : 0.0) << endr;
  if (myTopo != 0)
    modifier->initialize(myTopo,
      myPositions,
      myVelocities,
      myForces,
      myEnergies);
  myPostStepModifiers.insert(modifier);
  addModifier(modifier);
}

//____  ---------------------------------------------------------------------  //

void Integrator::deleteInternalModifiers() {
  report << debug(10) <<
  "[Integrator::deleteInternalModifiers]" << endr;
  for (set<Modifier *>::iterator i = myPreStepModifiers.begin();
       i != myPreStepModifiers.end();
       ++i)
    if ((*i)->isInternal()) {
      myPreStepModifiers.erase(i);
      deleteModifier(*i);
    }

  for (set<Modifier *>::iterator i = myPreDriftOrNextModifiers.begin();
       i != myPreDriftOrNextModifiers.end();
       ++i)
    if ((*i)->isInternal()) {
      myPreDriftOrNextModifiers.erase(i);
      deleteModifier(*i);
    }

  for (set<Modifier *>::iterator i = myPostDriftOrNextModifiers.begin();
       i != myPostDriftOrNextModifiers.end();
       ++i)
    if ((*i)->isInternal()) {
      myPostDriftOrNextModifiers.erase(i);
      deleteModifier(*i);
    }

  for (set<Modifier *>::iterator i = myPreForceModifiers.begin();
       i != myPreForceModifiers.end();
       ++i)
    if ((*i)->isInternal()) {
      myPreForceModifiers.erase(i);
      deleteModifier(*i);
    }

  for (set<Modifier *>::iterator i = myMediForceModifiers.begin();
       i != myMediForceModifiers.end();
       ++i)
    if ((*i)->isInternal()) {
      myMediForceModifiers.erase(i);
      deleteModifier(*i);
    }

  for (set<Modifier *>::iterator i = myPostForceModifiers.begin();
       i != myPostForceModifiers.end();
       ++i)
    if ((*i)->isInternal()) {
      myPostForceModifiers.erase(i);
      deleteModifier(*i);
    }

  for (set<Modifier *>::iterator i = myPostStepModifiers.begin();
       i != myPostStepModifiers.end();
       ++i)
    if ((*i)->isInternal()) {
      myPostStepModifiers.erase(i);
      deleteModifier(*i);
    }
}

//____  ---------------------------------------------------------------------  //

void Integrator::deleteExternalModifiers() {
  report << debug(10) <<
  "[Integrator::deleteExternalModifiers] size=" << myListModifiers.size() <<
  endr;
  for (set<Modifier *>::iterator i = myPreStepModifiers.begin();
       i != myPreStepModifiers.end();
       ++i)
    if (!((*i)->isInternal())) {
      myPreStepModifiers.erase(i);
      deleteModifier(*i);
    }

  for (set<Modifier *>::iterator i = myPreDriftOrNextModifiers.begin();
       i != myPreDriftOrNextModifiers.end();
       ++i)
    if (!((*i)->isInternal())) {
      myPreDriftOrNextModifiers.erase(i);
      deleteModifier(*i);
    }

  for (set<Modifier *>::iterator i = myPostDriftOrNextModifiers.begin();
       i != myPostDriftOrNextModifiers.end();
       ++i)
    if (!((*i)->isInternal())) {
      myPostDriftOrNextModifiers.erase(i);
      deleteModifier(*i);
    }

  for (set<Modifier *>::iterator i = myPreForceModifiers.begin();
       i != myPreForceModifiers.end();
       ++i)
    if (!((*i)->isInternal())) {
      myPreForceModifiers.erase(i);
      deleteModifier(*i);
    }

  for (set<Modifier *>::iterator i = myMediForceModifiers.begin();
       i != myMediForceModifiers.end();
       ++i)
    if (!((*i)->isInternal())) {
      myMediForceModifiers.erase(i);
      deleteModifier(*i);
    }

  for (set<Modifier *>::iterator i = myPostForceModifiers.begin();
       i != myPostForceModifiers.end();
       ++i)
    if (!((*i)->isInternal())) {
      myPostForceModifiers.erase(i);
      deleteModifier(*i);
    }

  for (set<Modifier *>::iterator i = myPostStepModifiers.begin();
       i != myPostStepModifiers.end();
       ++i)
    if (!((*i)->isInternal())) {
      myPostStepModifiers.erase(i);
      deleteModifier(*i);
    }

  report << debug(10) <<
  "[Integrator::deleteExternalModifiers] end size=" <<
  myListModifiers.size() << endr;
}

//____  ---------------------------------------------------------------------  //

bool Integrator::removeModifier(const Modifier *modifier) {
  report << debug(10) << "[Integrator::removeModifier]" <<
  endr;
  bool ok = false;
  for (set<Modifier *>::iterator i = myPreStepModifiers.begin();
       i != myPreStepModifiers.end();
       ++i)
    if (modifier == (*i)) {
      myPreStepModifiers.erase(i);
      ok = true;
    }

  for (set<Modifier *>::iterator i = myPreDriftOrNextModifiers.begin();
       i != myPreDriftOrNextModifiers.end();
       ++i)
    if (modifier == (*i)) {
      myPreDriftOrNextModifiers.erase(i);
      ok = true;
    }

  for (set<Modifier *>::iterator i = myPostDriftOrNextModifiers.begin();
       i != myPostDriftOrNextModifiers.end();
       ++i)
    if (modifier == (*i)) {
      myPostDriftOrNextModifiers.erase(i);
      ok = true;
    }

  for (set<Modifier *>::iterator i = myPreForceModifiers.begin();
       i != myPreForceModifiers.end();
       ++i)
    if (modifier == (*i)) {
      myPreForceModifiers.erase(i);
      ok = true;
    }

  for (set<Modifier *>::iterator i = myMediForceModifiers.begin();
       i != myMediForceModifiers.end();
       ++i)
    if (modifier == (*i)) {
      myMediForceModifiers.erase(i);
      ok = true;
    }

  for (set<Modifier *>::iterator i = myPostForceModifiers.begin();
       i != myPostForceModifiers.end();
       ++i)
    if (modifier == (*i)) {
      myPostForceModifiers.erase(i);
      ok = true;
    }

  for (set<Modifier *>::iterator i = myPostStepModifiers.begin();
       i != myPostStepModifiers.end();
       ++i)
    if (modifier == (*i)) {
      myPostStepModifiers.erase(i);
      ok = true;
    }

  if (ok) {
    set<Modifier *>::iterator i =
      myListModifiers.find(const_cast<Modifier *>(modifier));
    if (i != myListModifiers.end())
      myListModifiers.erase(i);
  }

  return ok;
}

//____  ---------------------------------------------------------------------  //

void Integrator::initializeModifiers() {
  report << debug(10) <<
  "[Integrator::initializeModifiers] " <<
  (myTopo != 0 ? myTopo->time : 0.0) << endr;
  for (set<Modifier *>::iterator i = myListModifiers.begin();
       i != myListModifiers.end();
       ++i)
    (*i)->initialize(myTopo, myPositions, myVelocities, myForces, myEnergies);
}

//____  ---------------------------------------------------------------------  //

void Integrator::addModifier(Modifier *modifier) {
  report << debug(10) << "[Integrator::addModifier] size=";
  myListModifiers.insert(modifier);
  report << myListModifiers.size() << endr;
}

//____  ---------------------------------------------------------------------  //

void Integrator::deleteModifier(Modifier *modifier) {
  report << debug(10) <<
  "[Integrator::deleteModifier] size=" << myListModifiers.size() << "," <<
  modifier->isInternal() << endr;
  set<Modifier *>::iterator i = myListModifiers.find(modifier);
  if (i != myListModifiers.end()) {
    report << debug(10) <<
    "[Integrator::deleteModifier] delete" << (long)(modifier) << endr;
    delete modifier;
    myListModifiers.erase(i);
  }
  report << debug(10) <<
  "[Integrator::deleteModifier] end size=" << myListModifiers.size() << endr;
}

//____  --------------------------------------------------------------------  //
//____  The last modifier found with modifierName, is removed.                //
//____  --------------------------------------------------------------------  //

bool Integrator::removeModifier(const string modifierName) {
  report << debug(10) << "[Integrator::removeModifier]" <<
  endr;

  bool found = false;
  Modifier *foundMod = 0;

  for (set<Modifier *>::iterator i = myPostStepModifiers.begin();
       i != myPostStepModifiers.end();
       ++i)

    if ((*i)->print() == modifierName) {
      myPostStepModifiers.erase(i);
      foundMod = *i;
      found = true;
    }

  for (set<Modifier *>::iterator i = myPreStepModifiers.begin();
       i != myPreStepModifiers.end();
       ++i)

    if ((*i)->print() == modifierName) {
      myPreStepModifiers.erase(i);
      foundMod = *i;
      found = true;
    }

  for (set<Modifier *>::iterator i = myPreDriftOrNextModifiers.begin();
       i != myPreDriftOrNextModifiers.end();
       ++i)

    if ((*i)->print() == modifierName) {
      myPreDriftOrNextModifiers.erase(i);
      foundMod = *i;
      found = true;
    }

  for (set<Modifier *>::iterator i = myPostDriftOrNextModifiers.begin();
       i != myPostDriftOrNextModifiers.end();
       ++i)

    if ((*i)->print() == modifierName) {
      myPostDriftOrNextModifiers.erase(i);
      foundMod = *i;
      found = true;
    }

  for (set<Modifier *>::iterator i = myPreForceModifiers.begin();
       i != myPreForceModifiers.end();
       ++i)

    if ((*i)->print() == modifierName) {
      myPreForceModifiers.erase(i);
      foundMod = *i;
      found = true;
    }

  for (set<Modifier *>::iterator i = myMediForceModifiers.begin();
       i != myMediForceModifiers.end();
       ++i)

    if ((*i)->print() == modifierName) {
      myMediForceModifiers.erase(i);
      foundMod = *i;
      found = true;
    }

  for (set<Modifier *>::iterator i = myPostForceModifiers.begin();
       i != myPostForceModifiers.end();
       ++i)

    if ((*i)->print() == modifierName) {
      myPostForceModifiers.erase(i);
      foundMod = *i;
      found = true;
    }

  if (found) {
    set<Modifier *>::iterator i = myListModifiers.find(
      const_cast<Modifier *>(foundMod));

    if (i != myListModifiers.end())
      myListModifiers.erase(i);
  }

  return found;
}

//____  --------------------------------------------------------------------  //
//____  These methods will save/restore the forces.  This is probably only    //
//____  going to be used by *MCIntegrator methods, so it may one day be       //
//____  moved to a more appropriate position.                                 //
//____  --------------------------------------------------------------------  //

void Integrator::saveForces() {
  (*myOldForces) = (*myForces);
}

void Integrator::restoreForces() {
  (*myForces) = (*myOldForces);
}

