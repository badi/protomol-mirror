#ifndef TEMPLATE_MODULE_H
#define TEMPLATE_MODULE_H

#include <protomol/base/Module.h>
#include <protomol/config/InputValue.h>

#include <vector>
#include <string>

namespace ProtoMol {
  class ProtoMolApp;

  //declareInputValue(Input, STRING, NOTEMPTY);

  class TemplateModule : public Module {
  public:
    const std::string getName() const {return "Template";}
    int getPriority() const {return 0;}
    const std::string getHelp() const {
      return "";
    }

    void init(ProtoMolApp *app);
  };
};

#endif // TEMPLATE_MODULE_H
