#include "ProtoMolApp.h"

#include "MainModule.h"

#include <protomol/frontend/CommandLine.h>
#include <protomol/config/Configuration.h>
#include <protomol/config/ConfigurationModule.h>
#include <protomol/util/SystemUtilities.h>
#include <protomol/util/PMConstants.h>
#include <protomol/util/Report.h>

#include <protomol/types/String.h>
#include <protomol/types/PDB.h>

#include <protomol/io/PosVelReader.h>
#include <protomol/io/EigenvectorReader.h>
#include <protomol/io/PSFReader.h>
#include <protomol/io/PARReader.h>

#include <protomol/topology/GenericTopology.h>
#include <protomol/topology/TopologyFactory.h>
#include <protomol/topology/TopologyModule.h>

using namespace std;
using namespace ProtoMol;
using namespace ProtoMol::Report;
using namespace ProtoMol::Constant;


ProtoMolApp::ProtoMolApp(ModuleManager *modManager) :
  modManager(modManager), cmdLine(&config) {
  modManager->init(this);

  topologyFactory.registerAllExemplarsConfiguration(&config);
}

ProtoMolApp::~ProtoMolApp() {
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

void ProtoMolApp::read() {
  // Positions
  PosVelReader reader;
  if (!reader.open(config[InputPositions::keyword]))
    THROW(string("Can't open position file '") + 
          config[InputPositions::keyword].getString() + "'.");

  if (reader.tryFormat(PosVelReaderType::PDB)) {
    PDB pdb;
    if (!(reader >> pdb))
      THROW(string("Could not parse PDB position file '") +
            config[InputPositions::keyword].getString() + "'.");

    swap(positions, pdb.coords);

  } else if (!(reader >> positions))
    THROW(string("Could not parse position file '") +
          config[InputPositions::keyword].getString() +
          "'. Supported formats are : " +
          PosVelReaderType::getPossibleValues(", ") + ".");

  report << plain << "Using " << reader.getType() << " posfile '"
         << config[InputPositions::keyword] << "' ("
         << positions.size() << ")." << endr;


  // Velocities
  if (config.valid(InputVelocities::keyword)) {
    if (!reader.open(config[InputVelocities::keyword]))
      THROW(string("Can't open velocity file '") +
            config[InputVelocities::keyword].getString() + "'.");

    if (!(reader >> velocities))
      THROW(string("Could not parse velocity file '") +
            config[InputVelocities::keyword].getString() +
            "'. Supported formats are : " +
            PosVelReaderType::getPossibleValues(", ") + ".");

    report << plain << "Using " << reader.getType() << " velfile '"
           << config[InputVelocities::keyword] << "' ("
           << velocities.size() << ")." << endr;

    if (reader.getType() == "PDB" && (bool)config[InputPDBScaling::keyword]) {
      for (unsigned int i = 0; i < velocities.size(); i++)
        velocities[i] /= PDBVELSCALINGFACTOR;
      report << plain << "PDB velocities scaled." << endr;
    }

  } else if (config.valid(InputTemperature::keyword)) {    
    velocities.resize(positions.size());

    report << plain << "Using temperature "
           << config[InputTemperature::keyword] << "K for the velocities  ("
           << velocities.size() << ")." << endr;
    // Create velocities later, we need the topology for that ...

  } else THROW("Neither temperature nor velocity file specified.");


  // Eigenvectors/values
  if (config.valid(InputEigenVectors::keyword)) {
    EigenvectorReader evReader;

    if (!evReader.open(config[InputEigenVectors::keyword]))
      THROW(string("Can't open eigenvector file '") +
            config[InputEigenVectors::keyword].getString() + "'.");

    if (!(evReader >> eigenInfo)) {
      if (eigenInfo.myEigenvectorLength != (double)positions.size())
        THROW(string("Eigenvector length is wrong, should be ") +
              String(positions.size()) + " got " +
              String(eigenInfo.myEigenvectorLength) + ".");

      if (eigenInfo.myNumEigenvectors < 1 ||
          eigenInfo.myNumEigenvectors > (double)positions.size())
        THROW(string("Wrong number of eigenvectors (") +
              String(eigenInfo.myNumEigenvectors) + ").");
    }
    report << plain << "Using " << reader.getType() << " eigfile '"
           << config[InputEigenVectors::keyword] << "' ("
           << eigenInfo.myEigenvectorLength << ")." << endr;
  }


  // PSF
  PSFReader psfReader;
  if (!psfReader.open(config[InputPSF::keyword]))
    THROW(string("Can't open PSF file '") +
          config[InputPSF::keyword].getString() + "'.");

  if (!(psfReader >> psf))
    THROW(string("Could not parse PSF file '") +
          config[InputPSF::keyword].getString() + "'.");

  report << plain << "Using PSF file '" << config[InputPSF::keyword]
         << "' (" << psf.atoms.size() << ")." << endr;


  // PAR
  PARReader parReader;
  if (!parReader.open(config[InputPAR::keyword]))
    THROW(string("Can't open PAR file '") +
          config[InputPAR::keyword].getString() + "'.");

  if (!(parReader >> par))
    THROW(string("Could not parse PAR file '") +
          config[InputPAR::keyword].getString() + "'.");

  report << plain << "Using PAR file '" << config[InputPAR::keyword]
         << "', " << (parReader.getCharmmTypeDetected() != PAR::CHARMM28 ?
                    "old":"new") << " charmm force field.";

  if (!config[InputDihedralMultPSF::keyword].valid())
    config[InputDihedralMultPSF::keyword] =
      (parReader.getCharmmTypeDetected() != PAR::CHARMM28);

  if (config[InputDihedralMultPSF::keyword])
    report << " Dihedral multiplictity defined by PSF.";
  report << endr;
      

  // Test input
  if (positions.size() != velocities.size() ||
      positions.size() != psf.atoms.size())
    THROW("Positions, velocities and PSF input have different number "
          "of atoms.");
}

void ProtoMolApp::build() {
  // Create topology
  try {
    topology = topologyFactory.make(&config);

  } catch (const Exception &e) {
    // Try to get some defaults with the postions known ...
    const GenericTopology *prototype =
      topologyFactory.find(config[GenericTopology::keyword].getString());
    
    if (prototype) {
      vector<Parameter> parameters = prototype->getDefaults(positions);

      for (unsigned int i = 0; i < parameters.size(); i++) {
        if (!config.valid(parameters[i].keyword) &&
            parameters[i].value.valid()) {

          config.set(parameters[i].keyword, parameters[i].value);
          report << hint << parameters[i].keyword << " undefined, using "
                 << parameters[i].value.getString() << "." << endr;
        }
      }

      topology = topologyFactory.make(&config);
    }

    if (!topology) throw e;
  }

  // Build topology
  Module *topoMod = modManager->find("Topology");
  if (!topoMod) THROW("Topology module not found");

  topoMod->buildTopology(this);


  // Register Forces
  modManager->registerForces(this);  
}
