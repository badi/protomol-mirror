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


ProtoMolApp::ProtoMolApp(ModuleManager *modManager) : cmdLine(&config) {
  modManager->init(this);
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

  //  Set report level
  report << reportlevel((int)config[InputDebug::keyword]);

  // Fix for old topology definition
  if (!config[GenericTopology::keyword].valid()) {
    config[GenericTopology::keyword] =
      config[InputBoundaryConditions::keyword].getString() +
      config[InputCellManager::keyword].getString();
  }

  // Check if configuration is complete
  string errMsg;
  if (!config.validConfiguration(errMsg)) 
    report << plain << endl << errMsg << endr;
  
  if (!config[InputFirststep::keyword].valid())
    THROW("Firststep undefined.");
  if (!config[InputNumsteps::keyword].valid())
    THROW("Numsteps undefined.");

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
  string errMsg;  

  if (!(topology = TopologyFactory::make(errMsg, &config))) {
    // Try to get some defaults with the postions known ...
    const GenericTopology *prototype =
      TopologyFactory::find(config.get(GenericTopology::getKeyword()).
                            getString());

    if (prototype != NULL) {
      vector<Parameter> parameters = prototype->getDefaults(positions);

      for (unsigned int i = 0; i < parameters.size(); i++) {
        if (!config.valid(parameters[i].keyword) &&
            parameters[i].value.valid()) {

          config.set(parameters[i].keyword,parameters[i].value);
          report << hint << parameters[i].keyword << " undefined, using "
                 << parameters[i].value.getString() << "." << endr;
        }
      }

      topology = TopologyFactory::make(errMsg, &config);
    }

    if (!topology) THROW(errMsg);
  }


#if 0
    // Build topology
  if ((bool)config[InputDoSCPISM::keyword]) {
    cout << (bool)config[InputDoSCPISM::keyword]  << endl;
    topology->doSCPISM = true;
  }
  buildTopology(topology, psf, par, config[InputDihedralMultPSF::keyword]);

  topology->minimalMolecularDistances =
    topology->checkMoleculePairDistances(positions);

  if ((bool)config[InputReducedImage::keyword] &&
      !topology->minimalMolecularDistances) {
    Vector3DBlock tmp(positions);

    topology->minimalImage(tmp);

    if (topology->checkMoleculePairDistances(tmp)) {
      positions = tmp;
      report << plain << "Fixed minimal molecule distances." << endr;
      topology->minimalMolecularDistances = true;

    } else {
      report << plain << "Could not fixed minimal molecule distances." << endr;
      topology->minimalMolecularDistances = false;
    }      
  }


  // Fix velocities
  if (!config.valid(InputVelocities::keyword)) {
    randomVelocity(config[InputTemperature::keyword],
                   topology,&velocities,config[InputSeed::keyword]);
    report << plain << "Random temperature : "
           <<temperature(topology,&velocities)<<"K"<<endr;
  }

  if ((int)config[InputRemoveLinearMomentum::keyword] >= 0){
    report << plain << "Removed linear momentum: "
           << removeLinearMomentum(&velocities, topology) *
      Constant::INV_TIMEFACTOR
           <<endr;

  } else
    report << plain << "Linear momentum : "
           << linearMomentum(&velocities, topology) *
      Constant::INV_TIMEFACTOR
           <<endr;

  if ((int)config[InputRemoveAngularMomentum::keyword] >= 0){
    report << plain << "Removed angular momentum : "
	   <<removeAngularMomentum(&positions,&velocities, topology) *
      Constant::INV_TIMEFACTOR << endr;

  } else {
    report << plain << "Angular momentum : "
           << angularMomentum(&positions, &velocities, topology) *
      Constant::INV_TIMEFACTOR << endr;
  }
  report << plain << "Actual start temperature : "
         << temperature(topology, &velocities) << "K" << endr;
#endif
}
