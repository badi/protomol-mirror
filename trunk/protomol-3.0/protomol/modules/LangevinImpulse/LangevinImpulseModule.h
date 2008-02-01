#ifndef LANGEVINIMPULSE_MODULE_H
#define LANGEVINIMPULSE_MODULE_H

#include <protomol/base/Module.h>

#include <string>

namespace ProtoMol {
  class ProtoMolApp;

  class LangevinImpulseModule : public Module {
  public:
    const std::string getName() const {return "LangevinImpulse";}
    int getPriority() const {return 0;}
    const std::string getHelp() const {return "";}

    void init(ProtoMolApp *app);
  };
};

#endif // LANGEVINIMPULSE_MODULE_H
