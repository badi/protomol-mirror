#include <protomol/base/ModuleManager.h>

#include <protomol/frontend/MainModule.h>
#include <protomol/frontend/CommandLineModule.h>
#include <protomol/config/ConfigurationModule.h>

using namespace ProtoMol;

void moduleInitFunction(ModuleManager *manager) {
  manager->add(new MainModule());
  manager->add(new CommandLineModule());
  manager->add(new ConfigurationModule());
}
