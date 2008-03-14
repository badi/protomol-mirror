#include <protomol/output/Output.h>
#include <protomol/config/Configuration.h>
#include <protomol/topology/GenericTopology.h>
#include <protomol/type/ScalarStructure.h>
#include <protomol/module/MainModule.h>
#include <protomol/ProtoMolApp.h>

using namespace std;
using namespace ProtoMol;

//____ Output
const string Output::scope("Output");

Output::Output() :
  myFirstStep(0), myNextStep(0), myLastStep(0), myFirst(true),
  myOutputFreq(0), app(0)
{}

Output::Output(int freq) :
  myFirstStep(0), myNextStep(0), myLastStep(0), myFirst(true),
  myOutputFreq(freq), app(0)
{}

void Output::initialize(const ProtoMolApp *app) {
  myFirst = true;
  this->app = app;

  if (app->config.valid(InputFirststep::keyword)) {
    myNextStep = app->config[InputFirststep::keyword];
    myFirstStep = app->config[InputFirststep::keyword];
    myLastStep = myFirstStep;
  }

  if (app->config.valid(InputNumsteps::keyword))
    myLastStep =
      myLastStep + app->config[InputNumsteps::keyword].operator int();

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
    if (app->energies.output())
      doRun(step);
  }
  myFirst = false;
}

void Output::finalize(int step) {
  if (app->energies.output()) doRun(step);
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