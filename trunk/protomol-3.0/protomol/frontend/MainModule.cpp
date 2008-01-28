#include "MainModule.h"

#include "ProtoMolApp.h"

#include <protomol/config/Configuration.h>

using namespace std;
using namespace ProtoMol;

defineInputValue(InputSeed, "seed");
defineInputValue(InputFirststep, "firststep");
defineInputValue(InputNumsteps, "numsteps");
defineInputValueAndText(InputDebug, "debug", "report level, suppresses all "
                        "output with higher output level");
defineInputValueWithAliases(InputPositions, "posfile",
                            ("coords")("coordinates"));
defineInputValue(InputVelocities, "velfile");
defineInputValue(InputTemperature,"temperature");
defineInputValue(InputEigenVectors, "eigfile");
defineInputValueWithAliases(InputPSF, "psffile", ("structure"));
defineInputValueWithAliases(InputPAR, "parfile", ("parameters"));
defineInputValue(InputPDBScaling,"pdbScaling");
defineInputValue(InputDihedralMultPSF,"dihedralMultPSF");

void MainModule::init(ProtoMolApp *app) {
  Configuration *config = &app->getConfiguration();

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
  InputTemperature::registerConfiguration(config);
  InputEigenVectors::registerConfiguration(config);
  InputPSF::registerConfiguration(config);
  InputPAR::registerConfiguration(config);
  InputPDBScaling::registerConfiguration(config);
  InputDihedralMultPSF::registerConfiguration(config);
}
