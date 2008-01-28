#ifndef COMMAND_LINE_MODULE_H
#define COMMAND_LINE_MODULE_H

#include <protomol/base/Module.h>

namespace ProtoMol {
  class ProtoMolApp;

  class CommandLineModule : public Module {
  public:
    const std::string getName() const {return "CommandLine";}
    int getPriority() const {return 0;}
    const std::string getHelp() const {
      return "Adds standard command line options.";
    }

    void getDependencies(module_deps_t &deps) const {
      deps.insert("Configuration");
    }

    void init(ProtoMolApp *app);
  };
};

#endif // COMMAND_LINE_MODULE_H
