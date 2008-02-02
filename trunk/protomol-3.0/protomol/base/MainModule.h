#ifndef MAIN_MODULE_H
#define MAIN_MODULE_H

#include <protomol/base/Module.h>
#include <protomol/config/InputValue.h>

namespace ProtoMol {
  class ProtoMolApp;
  
  declareInputValue(InputNumsteps, INT, NOTNEGATIVE);
  declareInputValue(InputFirststep, INT, NOCONSTRAINTS);
  declareInputValue(InputSeed, INT, NOTNEGATIVE);
  declareInputValue(InputDebug, INT, NOCONSTRAINTS);
  declareInputValue(InputPositions, STRING, NOTEMPTY);
  declareInputValue(InputVelocities, STRING, NOTEMPTY);
  declareInputValue(InputEigenVectors, STRING, NOTEMPTY);
  declareInputValue(InputPSF, STRING, NOTEMPTY);
  declareInputValue(InputPAR, STRING, NOTEMPTY);
  declareInputValue(InputPDBScaling, BOOL, NOCONSTRAINTS);
  declareInputValue(InputDihedralMultPSF, BOOL, NOCONSTRAINTS);
  declareInputValue(InputIntegrator, INTEGRATOR, NOTEMPTY);

  class MainModule : public Module {
  public:
    const std::string getName() const {return "Main";}
    int getPriority() const {return 0;}
    const std::string getHelp() const {
      return "System default registrations.";
    }

    void init(ProtoMolApp *app);
    void configure(ProtoMolApp *app);
  };
};

#endif // MAIN_MODULE_H
