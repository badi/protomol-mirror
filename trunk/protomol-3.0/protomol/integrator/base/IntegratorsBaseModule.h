#ifndef INTEGRATOR_BASE_MODULE_H
#define INTEGRATOR_BASE_MODULE_H

#include <protomol/base/Module.h>

#include <string>

namespace ProtoMol {
  class ProtoMolApp;

  class IntegratorBaseModule : public Module {
  public:
    const std::string getName() const {return "IntegratorBase";}
    int getPriority() const {return 0;}
    const std::string getHelp() const {return "";}

    void init(ProtoMolApp *app);
  };
};

#endif // INTEGRATOR_BASE_MODULE_H
