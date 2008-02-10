#include <protomol/base/ModuleManager.h>

#include <protomol/modules/MainModule.h>
#include <protomol/modules/CommandLineModule.h>
#include <protomol/modules/ConfigurationModule.h>
#include <protomol/modules/TopologyModule.h>
#include <protomol/modules/OutputModule.h>
#include <protomol/modules/BondedForcesModule.h>
#include <protomol/modules/ModifierModule.h>

#include <protomol/modules/IntegratorBaseModule.h>
#include <protomol/modules/NormalModeModule.h>
#include <protomol/modules/LeapfrogModule.h>
#include <protomol/modules/HessianIntegratorModule.h>

#include <protomol/modules/NonbondedCutoffForceModule.h>
#include <protomol/modules/NonbondedFullForceModule.h>
#include <protomol/modules/NonbondedSimpleFullForceModule.h>

using namespace ProtoMol;

void moduleInitFunction(ModuleManager *manager) {
  manager->add(new MainModule());
  manager->add(new CommandLineModule());
  manager->add(new ConfigurationModule());
  manager->add(new TopologyModule());
  manager->add(new OutputModule());
  manager->add(new ModifierModule());

  // Integrators
  manager->add(new IntegratorBaseModule());
  manager->add(new NormalModeModule());
  manager->add(new LeapfrogModule());
  manager->add(new HessianIntegratorModule());

  // Forces
  manager->add(new BondedForcesModule());
  manager->add(new NonbondedCutoffForceModule());
  manager->add(new NonbondedFullForceModule());
  manager->add(new NonbondedSimpleFullForceModule());
}

