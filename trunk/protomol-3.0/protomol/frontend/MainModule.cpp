#include "MainModule.h"

#include "ProtoMolApp.h"

#include <protomol/config/Configuration.h>
#include <protomol/util/Report.h>
#include <protomol/debug/Exception.h>

using namespace std;
using namespace ProtoMol;
using namespace ProtoMol::Report;

defineInputValue(InputSeed, "seed");
defineInputValue(InputFirststep, "firststep");
defineInputValue(InputNumsteps, "numsteps");
defineInputValueAndText(InputDebug, "debug", "report level, suppresses all "
                                             "output with higher output level");
defineInputValueWithAliases(InputPositions, "posfile",
  ("coords")("coordinates"));
defineInputValue(InputVelocities, "velfile");
defineInputValue(InputEigenVectors, "eigfile");
defineInputValueWithAliases(InputPSF, "psffile", ("structure"));
defineInputValueWithAliases(InputPAR, "parfile", ("parameters"));
defineInputValue(InputPDBScaling, "pdbScaling");
defineInputValue(InputDihedralMultPSF, "dihedralMultPSF");
defineInputValue(InputIntegrator, "integrator");

void MainModule::init(ProtoMolApp *app) {
  Configuration *config = &app->config;

  InputSeed::registerConfiguration(config);
  InputFirststep::registerConfiguration(config);
  InputNumsteps::registerConfiguration(config);
#ifdef DEBUG
  InputDebug::registerConfiguration(config, 1);
#else
  InputDebug::registerConfiguration(config, 0);
#endif

  InputPositions::registerConfiguration(config);
  InputVelocities::registerConfiguration(config);
  InputEigenVectors::registerConfiguration(config);
  InputPSF::registerConfiguration(config);
  InputPAR::registerConfiguration(config);
  InputPDBScaling::registerConfiguration(config);
  InputDihedralMultPSF::registerConfiguration(config);
  InputIntegrator::registerConfiguration(config);
}

void MainModule::configure(ProtoMolApp *app) {
  Configuration &config = app->config;

  //  Set report level
  report << reportlevel((int)config[InputDebug::keyword]);

  // Check if configuration is complete
  if (config.hasUndefinedKeywords()) {
    report << plain << "Undefined Keyword(s):" << endl
           << config.printUndefinedKeywords() << endl;
    
    if (!config[InputFirststep::keyword].valid())
      THROW("Firststep undefined.");
    
    if (!config[InputNumsteps::keyword].valid())
      THROW("Numsteps undefined.");
  }
}

