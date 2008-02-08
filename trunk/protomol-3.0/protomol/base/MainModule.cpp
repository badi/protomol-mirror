#include <protomol/base/MainModule.h>

#include <protomol/base/ProtoMolApp.h>
#include <protomol/topology/TopologyUtilities.h>
#include <protomol/config/Configuration.h>
#include <protomol/base/Report.h>
#include <protomol/base/Exception.h>

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
defineInputValue(InputReducedImage, "reducedImage");
defineInputValue(InputTemperature, "temperature");
defineInputValue(InputDoSCPISM, "doscpism");

void MainModule::init(ProtoMolApp *app) {
  Configuration *config = &app->config;

  InputSeed::registerConfiguration(config);
  InputFirststep::registerConfiguration(config, 0);
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
  InputReducedImage::registerConfiguration(config);
  InputTemperature::registerConfiguration(config);
  InputDoSCPISM::registerConfiguration(config);
}

void MainModule::configure(ProtoMolApp *app) {
  Configuration &config = app->config;

  //  Set report level
  report << reportlevel((int)config[InputDebug::keyword]);

  // Check if configuration is complete
  if (config.hasUndefinedKeywords()) {
    report << debug(2) << "Undefined Keyword(s):" << endl
           << config.printUndefinedKeywords() << endr;
  }
    
  if (!config[InputFirststep::keyword].valid())
    THROW("Firststep undefined.");
  
  if (!config[InputNumsteps::keyword].valid())
    THROW("Numsteps undefined.");
}

void MainModule::postBuild(ProtoMolApp *app) {  
  // Reduce image
  app->topology->minimalMolecularDistances =
    app->topology->checkMoleculePairDistances(app->positions);

  if ((bool)app->config[InputReducedImage::keyword] &&
      !app->topology->minimalMolecularDistances) {
    Vector3DBlock tmp(app->positions);

    app->topology->minimalImage(tmp);

    if (app->topology->checkMoleculePairDistances(tmp)) {
      app->positions = tmp;
      report << plain << "Fixed minimal molecule distances." << endr;
      app->topology->minimalMolecularDistances = true;

    } else {
      report << plain << "Could not fixed minimal molecule distances." << endr;
      app->topology->minimalMolecularDistances = false;
    }
  }

   // Fix velocities
  if (!app->config.valid(InputVelocities::keyword)) {
    randomVelocity(app->config[InputTemperature::keyword],
                   app->topology, &app->velocities,
                   app->config[InputSeed::keyword]);

    report << plain << "Random temperature : "
           << temperature(app->topology, &app->velocities) << "K" << endr;
  }
}
