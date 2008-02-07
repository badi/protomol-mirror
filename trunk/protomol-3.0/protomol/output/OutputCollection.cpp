#include <protomol/output/OutputCollection.h>
#include <protomol/output/Output.h>
#include <protomol/config/Configuration.h>
#include <protomol/topology/GenericTopology.h>
#include <protomol/type/ScalarStructure.h>
#include <protomol/type/Vector3DBlock.h>
#include <protomol/integrator/Integrator.h>
#include <protomol/base/Report.h>
#include <protomol/base/MathUtilities.h>
#include <protomol/topology/TopologyUtilities.h>
#include <protomol/base/ProtoMolApp.h>

using namespace ProtoMol;
//____ OutputCollection
OutputCollection::OutputCollection() :
  cache(0), myConfig(0), myTopology(0), myIntegrator(0), myEnergies(0),
  myPositions(0), myVelocities(0) {}

OutputCollection::~OutputCollection() {
  for (iterator i = begin(); i != end(); ++i)
    delete (*i);
}

void OutputCollection::initialize(const ProtoMolApp *app) {
  cache = &app->outputCache;
  myConfig = &app->config;
  myTopology = app->topology;
  myIntegrator = app->integrator;
  myEnergies = &app->energies;
  myPositions = &app->positions;
  myVelocities = &app->velocities;

  for (iterator i = begin(); i != end(); ++i)
    (*i)->initialize(app);
}

void OutputCollection::run(int step) {
  cache->uncache();
  for (iterator i = begin(); i != end(); ++i)
    (*i)->run(step);
}

void OutputCollection::updateNextStep(int step) {
  cache->uncache();
  for (iterator i = begin(); i != end(); ++i)
    (*i)->updateNextStep(step);
}

void OutputCollection::finalize(int step) {
  cache->uncache();
  for (iterator i = begin(); i != end(); ++i)
    (*i)->finalize(step);
}

void OutputCollection::adoptOutput(Output *output) {
  if (output != 0) {
    output->setCache(cache);
    myOutputList.push_back(output);
  }
}

int OutputCollection::getNext() const {
  int next = Constant::MAX_INT;
  for (const_iterator i = begin(); i != end(); ++i)
    next = min((*i)->getNext(), next);

  return next;
}
