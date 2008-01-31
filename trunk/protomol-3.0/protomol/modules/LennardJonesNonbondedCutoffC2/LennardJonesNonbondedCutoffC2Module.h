#ifndef LENNARDJONESNONBONDEDCUTOFFC2_MODULE_H
#define LENNARDJONESNONBONDEDCUTOFFC2_MODULE_H

#include <protomol/base/Module.h>

#include <string>

namespace ProtoMol {
  class ProtoMolApp;

  class LennardJonesNonbondedCutoffC2Module : public Module {
  public:
    const std::string getName() const {return "LennardJonesNonbondedCutoffC2";}
    int getPriority() const {return 0;}
    const std::string getHelp() const {return "";}

    void init(ProtoMolApp *app) {}
    void registerForces(ProtoMolApp *app);
  };
};

#endif // LENNARDJONESNONBONDEDCUTOFFC2_MODULE_H
