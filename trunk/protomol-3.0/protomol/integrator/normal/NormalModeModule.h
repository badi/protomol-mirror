#ifndef NORMALMODE_MODULE_H
#define NORMALMODE_MODULE_H

#include <protomol/base/Module.h>

#include <string>

namespace ProtoMol {
  class ProtoMolApp;

  class NormalModeModule : public Module {
  public:
    const std::string getName() const {return "NormalMode";}
    void init(ProtoMolApp *app);
  };
};

#endif // NORMALMODE_MODULE_H
