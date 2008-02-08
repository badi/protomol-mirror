#ifndef MODIFIER_MODULE_H
#define MODIFIER_MODULE_H

#include <protomol/base/Module.h>

#include <string>

namespace ProtoMol {
  class ProtoMolApp;

  class ModifierModule : public Module {
  public:
    const std::string getName() const {return "Modifier";}
    int getPriority() const {return 0;}
    const std::string getHelp() const {return "";}

    void init(ProtoMolApp *app);
  };
};

#endif // MODIFIER_MODULE_H
