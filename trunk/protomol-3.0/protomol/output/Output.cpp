#include <protomol/output/Output.h>
#include <protomol/config/Configuration.h>
#include <protomol/topology/GenericTopology.h>
#include <protomol/types/ScalarStructure.h>
#include <protomol/frontend/MainModule.h>
#include <protomol/frontend/ProtoMolApp.h>

using namespace std;
using namespace ProtoMol;

//____ Output
const string Output::scope("Output");

Output::Output() :
  Makeable(), myFirstStep(0), myLastStep(0), myNextStep(0), myFirst(true),
  myOutputFreq(0), cache(0), myConfig(0), myTopology(0), myIntegrator(0),
  myEnergies(0), myPositions(0), myVelocities(0)
{}

Output::Output(int freq) :
  Makeable(), myFirstStep(0), myLastStep(0), myNextStep(0), myFirst(true),
  myOutputFreq(freq), cache(0), myConfig(0), myTopology(0), myIntegrator(0),
  myEnergies(0), myPositions(0), myVelocities(0)
{}

void Output::initialize(const ProtoMolApp *app) {
  myFirst = true;
  cache = &app->outputCache;
  myConfig = &app->config;
  myTopology = app->topology;
  myIntegrator = app->integrator;
  myEnergies = &app->energies;
  myPositions = &app->positions;
  myVelocities = &app->velocities;

  if (myConfig->valid(InputFirststep::keyword)) {
    myNextStep = (*myConfig)[InputFirststep::keyword];
    myFirstStep = (*myConfig)[InputFirststep::keyword];
    myLastStep = myFirstStep;
  }

  if (myConfig->valid(InputNumsteps::keyword))
    myLastStep =
      myLastStep + (*myConfig)[InputNumsteps::keyword].operator int();

  doInitialize();
}

void Output::updateNextStep(int step) {
  int n = (step - myNextStep) / myOutputFreq;
  myNextStep += max(n, 1) * myOutputFreq;
}

void Output::run(int step) {
  if (step >= myNextStep) {
    int n = (step - myNextStep) / myOutputFreq;
    myNextStep += max(n, 1) * myOutputFreq;
    if (myEnergies->output())
      doRun(step);
  }
  myFirst = false;
}

void Output::finalize(int step) {
  if (myEnergies->output()) doRun(step);
  doFinalize(step);
}

Output *Output::make(const vector<Value> &values) const {
  assertParameters(values);
  return adjustAlias(doMake(values));
}

bool Output::isIdDefined(const Configuration *config) const {
  if (!addDoKeyword()) return config->valid(getId());

  string str("do" + getId());
  if (!config->valid(getId()) || config->empty(str)) return false;

  if (!config->valid(str)) return true;

  return (bool)(*config)[str];
}
