#include "TopologyModule.h"

#include "Topology.h"
#include "TopologyFactory.h"
#include "CubicCellManager.h"
#include "VacuumBoundaryConditions.h"
#include "PeriodicBoundaryConditions.h"

#include <protomol/frontend/ProtoMolApp.h>

using namespace std;
using namespace ProtoMol;

defineInputValue(InputBoundaryConditions, "boundaryConditions");
defineInputValue(InputCellManager, "cellManager");

void TopologyModule::init(ProtoMolApp *app) {
  Configuration *config = &app->getConfiguration();
  GenericTopology *topo;

  // vacuum or normal boundary conditions
  topo = new Topology<VacuumBoundaryConditions, CubicCellManager>();
  TopologyFactory::registerExemplar(topo, Vector<std::string>("NormalCubic"));

  // periodic boundary conditions
  topo = new Topology<PeriodicBoundaryConditions, CubicCellManager>();
  TopologyFactory::registerExemplar(topo);


  TopologyFactory::registerAllExemplarsConfiguration(config);

  InputBoundaryConditions::registerConfiguration(config);
  InputCellManager::registerConfiguration(config);
}
