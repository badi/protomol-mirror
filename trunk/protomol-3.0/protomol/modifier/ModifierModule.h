#ifndef MODIFIER_MODULE_H
#define MODIFIER_MODULE_H

#include <protomol/base/Module.h>
#include <protomol/config/InputValue.h>

#include <string>

namespace ProtoMol {
  class ProtoMolApp;

  declareInputValue(InputRemoveAngularMomentum, INT, NOCONSTRAINTS);
  declareInputValue(InputRemoveLinearMomentum, INT, NOCONSTRAINTS);

  class ModifierModule : public Module {
  public:
    const std::string getName() const {return "Modifier";}
    int getPriority() const {return 0;}
    const std::string getHelp() const {return "";}

    void init(ProtoMolApp *app);
    void postBuild(ProtoMolApp *app);
  };
};

#endif // MODIFIER_MODULE_H
