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
  declareInputValue(InputRemoveAngularMomentum, INT, NOCONSTRAINTS);
  declareInputValue(InputRemoveLinearMomentum, INT, NOCONSTRAINTS);

  class TopologyModule : public Module {
  public:
    const std::string getName() const {return "Topology";}
    int getPriority() const {return 0;}
    const std::string getHelp() const {
      return "";
    }

    // Module interface
    void init(ProtoMolApp *app);
    void configure(ProtoMolApp *app);
    void buildTopology(ProtoMolApp *app);

    void buildExclusionTable(GenericTopology *topo,
                             const ExclusionType &exclusionType);
    void buildTopology(GenericTopology *topo, const PSF &psf, const PAR &par,
                       bool dihedralMultPSF);
    void buildMoleculeTable(GenericTopology *topo);
  };
};

#endif // TOPOLOGY_MODULE_H
