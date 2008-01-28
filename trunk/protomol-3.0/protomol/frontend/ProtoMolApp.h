#ifndef PROTOMOLAPP_H
#define PROTOMOLAPP_H

#include <protomol/base/ModuleManager.h>
#include <protomol/config/Configuration.h>
#include <protomol/frontend/CommandLine.h>

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
    CommandLine cmdLine;
    Configuration config;

  public:
    Vector3DBlock positions;
    Vector3DBlock velocities;
    EigenvectorInfo eigenInfo;
    PSF psf;
    PAR par;
    ScalarStructure scalar;

    OutputCollection *outputs;
    Integrator *integrator;
    GenericTopology *topology;

    ProtoMolApp(ModuleManager *modManager);
    ~ProtoMolApp();

    CommandLine &getCommandLine() {return cmdLine;}
    Configuration &getConfiguration() {return config;}

    bool configure(int argc = 0, char *argv[] = 0);
    void read();
    void build();
  };
};
#endif // PROTOMOLAPP_H
