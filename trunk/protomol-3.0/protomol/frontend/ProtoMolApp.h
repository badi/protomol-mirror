#ifndef PROTOMOLAPP_H
#define PROTOMOLAPP_H

#include <protomol/base/ModuleManager.h>
#include <protomol/config/Configuration.h>
#include <protomol/frontend/CommandLine.h>

#include <protomol/topology/TopologyFactory.h>
#include <protomol/force/ForceFactory.h>
#include <protomol/integrator/IntegratorFactory.h>

#include <protomol/types/Vector3DBlock.h>
#include <protomol/types/EigenvectorInfo.h>
#include <protomol/types/PSF.h>
#include <protomol/types/PAR.h>
#include <protomol/types/ScalarStructure.h>

namespace ProtoMol {
  class OutputCollection;
  class Integrator;
  class GenericTopology;

  class ProtoMolApp {
    ModuleManager *modManager;

    CommandLine cmdLine;
    Configuration config;

  public:
    Vector3DBlock positions;
    Vector3DBlock velocities;
    EigenvectorInfo eigenInfo;
    PSF psf;
    PAR par;
    ScalarStructure scalar;

    TopologyFactory topologyFactory;
    ForceFactory forceFactory;
    IntegratorFactory integratorFactory;

    OutputCollection *outputs;
    Integrator *integrator;
    GenericTopology *topology;

    int currentStep;
    int lastStep;

    ProtoMolApp(ModuleManager *modManager);
    ~ProtoMolApp();

    CommandLine &getCommandLine() {return cmdLine;}
    Configuration &getConfiguration() {return config;}

    bool configure(int argc = 0, char *argv[] = 0);
    void read();
    void build();
    bool step(int inc = 1);
  };
};
#endif // PROTOMOLAPP_H
