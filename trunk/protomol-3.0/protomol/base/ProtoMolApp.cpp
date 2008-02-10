#include <protomol/base/ProtoMolApp.h>

#include <protomol/base/ModuleManager.h>
#include <protomol/modules/MainModule.h>
#include <protomol/modules/IOModule.h>

#include <protomol/config/CommandLine.h>
#include <protomol/config/Configuration.h>
#include <protomol/modules/ConfigurationModule.h>
#include <protomol/config/ConfigurationReader.h>
#include <protomol/base/SystemUtilities.h>
#include <protomol/base/PMConstants.h>
#include <protomol/base/Report.h>

#include <protomol/type/String.h>

#include <protomol/topology/GenericTopology.h>
#include <protomol/factory/TopologyFactory.h>
#include <protomol/topology/BuildTopology.h>
#include <protomol/topology/TopologyUtilities.h>

#include <protomol/factory/OutputFactory.h>
#include <protomol/output/OutputCollection.h>

using namespace std;
using namespace ProtoMol;
using namespace ProtoMol::Report;
using namespace ProtoMol::Constant;

ProtoMolApp::ProtoMolApp(ModuleManager *modManager) :
  modManager(modManager), cmdLine(&config), outputs(0), integrator(0),
  topology(0) {
  modManager->init(this);

  topologyFactory.registerAllExemplarsConfiguration(&config);
  outputFactory.registerAllExemplarsConfiguration(&config);
}

ProtoMolApp::~ProtoMolApp() {}

void ProtoMolApp::configure(const string &configfile) {
  const char *argv[] = {"ProtoMol", configfile.c_str(), 0};

  configure(2, (char **)argv);
}

bool ProtoMolApp::configure(int argc, char *argv[]) {
  // Parse command line
  if (cmdLine.parse(argc, argv)) return false;

  // Read Config file
  if (config.valid(InputConfig::keyword))
    changeDirectory(config[InputConfig::keyword]);
  else THROW("Configuration file not set.");

  modManager->configure(this);

  return true;
}

void ProtoMolApp::build() {
  // Read data
  modManager->read(this);


  // Build topology
  try {
    topology = topologyFactory.make(&config);
  } catch (const Exception &e) {
    // Try to get some defaults with the postions known ...
    const GenericTopology *prototype =
      topologyFactory.find(config[GenericTopology::keyword].getString());

    if (prototype) {
      vector<Parameter> parameters = prototype->getDefaults(positions);

      for (unsigned int i = 0; i < parameters.size(); i++)
        if (!config.valid(parameters[i].keyword) &&
            parameters[i].value.valid()) {
          config.set(parameters[i].keyword, parameters[i].value);
          report << hint << parameters[i].keyword << " undefined, using "
                 << parameters[i].value.getString() << "." << endr;
        }

      topology = topologyFactory.make(&config);
    }

    if (!topology) throw e;
  }

  if (config[InputDoSCPISM::keyword]) {
    report << config[InputDoSCPISM::keyword] << endr;
    topology->doSCPISM = true;
  }

  buildTopology(topology, psf, par, config[InputDihedralMultPSF::keyword]);


  // Register Forces
  modManager->registerForces(this);


  // Build the integrators and forces
  integrator =
    integratorFactory.make(config[InputIntegrator::keyword], &forceFactory);


  // Create outputs
  // TODO if !Parallel::iAmMaster() turn off outputs
  if (config[InputOutput::keyword])
    outputs = outputFactory.makeCollection(&config);

  else outputs = new OutputCollection; // Empty collection


  // Post build processing
  modManager->postBuild(this);

  report << plain << "Actual start temperature : "
         << temperature(topology, &velocities) << "K" << endr;

  // Add Integrator Modifiers
  modManager->addModifiers(this);

  // Initialize
  energies.molecularVirial(config[InputMolVirialCalc::keyword]);
  energies.virial(config[InputVirialCalc::keyword]);
  report << plain << "Virial tensor : " << energies.virial() << endr;
  report << plain << "Molecular virial tensor : "
         << energies.molecularVirial() << endr;

  topology->time =
    (Real)config[InputFirststep::keyword] * integrator->getTimestep();

  integrator->initialize(this);
  outputs->initialize(this);
  outputCache.initialize(this);

  // Init cache
  //outputs->addToCache(pdbAtoms);
  outputCache.add(psf);
  outputCache.add(par);

  // Setup run
  currentStep = config[InputFirststep::keyword];
  lastStep = currentStep + (int)config[InputNumsteps::keyword];
}

bool ProtoMolApp::step() {
  if (currentStep >= lastStep) return false;

  outputs->run(currentStep);

  int inc = outputs->getNext() - currentStep;
  inc = std::min(lastStep, currentStep + inc) - currentStep;
  currentStep += inc;

  integrator->run(inc);

  return true;
}

void ProtoMolApp::finalize() {
  outputs->finalize(lastStep);

  // Clean up
  delete topology;
  delete integrator;
  delete outputs;
}
