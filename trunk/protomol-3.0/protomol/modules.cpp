#include <protomol/base/ModuleManager.h>

#include <protomol/base/MainModule.h>
#include <protomol/config/CommandLineModule.h>
#include <protomol/config/ConfigurationModule.h>
#include <protomol/topology/TopologyModule.h>
#include <protomol/output/OutputModule.h>

#include <protomol/modules/LangevinImpulse/LangevinImpulseModule.h>
#include <protomol/modules/BondedForces/BondedForcesModule.h>
#include <protomol/modules/LennardJonesNonbondedCutoffC2/LennardJonesNonbondedCutoffC2Module.h>

using namespace ProtoMol;

void moduleInitFunction(ModuleManager *manager) {
  manager->add(new MainModule());
  manager->add(new CommandLineModule());
  manager->add(new ConfigurationModule());
  manager->add(new TopologyModule());
  manager->add(new OutputModule());

  manager->add(new LangevinImpulseModule());
  manager->add(new BondedForcesModule());
  manager->add(new LennardJonesNonbondedCutoffC2Module());
}

