#include <protomol/base/ModuleManager.h>

#include <protomol/base/MainModule.h>
#include <protomol/config/CommandLineModule.h>
#include <protomol/config/ConfigurationModule.h>
#include <protomol/topology/TopologyModule.h>
#include <protomol/output/OutputModule.h>
#include <protomol/force/bonded/BondedForcesModule.h>
#include <protomol/modifier/ModifierModule.h>

#include <protomol/integrator/base/IntegratorBaseModule.h>

#include <protomol/modules/LennardJonesNonbondedCutoffC2/LennardJonesNonbondedCutoffC2Module.h>

using namespace ProtoMol;

void moduleInitFunction(ModuleManager *manager) {
  manager->add(new MainModule());
  manager->add(new CommandLineModule());
  manager->add(new ConfigurationModule());
  manager->add(new TopologyModule());
  manager->add(new OutputModule());
  manager->add(new ModifierModule());

  manager->add(new IntegratorBaseModule());
  manager->add(new BondedForcesModule());

  manager->add(new LennardJonesNonbondedCutoffC2Module());
}

