#ifndef PROTOMOLAPP_H
#define PROTOMOLAPP_H

#include <protomol/base/ModuleManager.h>
#include <protomol/config/Configuration.h>
#include <protomol/config/CommandLine.h>

#include <protomol/topology/TopologyFactory.h>
#include <protomol/force/ForceFactory.h>
#include <protomol/integrator/IntegratorFactory.h>

#include <protomol/output/OutputCache.h>
#include <protomol/output/OutputFactory.h>

#include <protomol/types/Vector3DBlock.h>
#include <protomol/types/EigenvectorInfo.h>
#include <protomol/types/PSF.h>
#include <protomol/types/PAR.h>
#include <protomol/types/ScalarStructure.h>

#include <ostream>

namespace ProtoMol {
  class OutputCollection;
  class Integrator;
  class GenericTopology;

  class ProtoMolApp {
    ModuleManager *modManager;

  public:
    // Data
    Vector3DBlock positions;
    Vector3DBlock velocities;
    EigenvectorInfo eigenInfo;
    PSF psf;
    PAR par;
    ScalarStructure energies;

    // Factories
    TopologyFactory topologyFactory;
    ForceFactory forceFactory;
    IntegratorFactory integratorFactory;
    OutputFactory outputFactory;

    // Containers
    CommandLine cmdLine;
    Configuration config;
    mutable OutputCache outputCache;
    OutputCollection *outputs;
    Integrator *integrator;
    GenericTopology *topology;

    // Run
    int currentStep;
    int lastStep;

    ProtoMolApp(ModuleManager *modManager);
    ~ProtoMolApp();

    void configure(const std::string &configfile);
    bool configure(int argc = 0, char *argv[] = 0);
    void read();
    void build();
    void print(std::ostream &stream);
    bool step();
    void finalize();
  };
};
#endif // PROTOMOLAPP_H
