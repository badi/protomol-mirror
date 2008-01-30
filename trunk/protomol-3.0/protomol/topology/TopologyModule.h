#ifndef TOPOLOGY_MODULE_H
#define TOPOLOGY_MODULE_H

#include <protomol/base/Module.h>
#include <protomol/config/InputValue.h>
#include <protomol/topology/ExclusionType.h>

#include <vector>
#include <string>

namespace ProtoMol {
  class ProtoMolApp;
  class GenericTopology;
  class PSF;
  class PAR;

  declareInputValue(InputBoundaryConditions, STRING, NOTEMPTY);
  declareInputValue(InputCellManager, STRING, NOTEMPTY);
  declareInputValue(InputDoSCPISM, BOOL, NOCONSTRAINTS);
  declareInputValue(InputReducedImage, BOOL, NOCONSTRAINTS);
  declareInputValue(InputTemperature, REAL, NOTNEGATIVE);
  declareInputValue(InputRemoveAngularMomentum, INT, NOCONSTRAINTS);
  declareInputValue(InputRemoveLinearMomentum, INT, NOCONSTRAINTS);

  class TopologyModule : public Module {
    GenericTopology *topo;

  public:
    const std::string getName() const {return "Topology";}
    int getPriority() const {return 0;}
    const std::string getHelp() const {
      return "";
    }

    // Module interface
    void init(ProtoMolApp *app);
    void configure(ProtoMolApp *app);
    GenericTopology *buildTopology(ProtoMolApp *app);

    void buildExclusionTable(const ExclusionType &exclusionType);
    void buildTopology(const PSF &psf, const PAR &par, bool dihedralMultPSF);
    void buildMoleculeTable();
  };
};

#endif // TOPOLOGY_MODULE_H
