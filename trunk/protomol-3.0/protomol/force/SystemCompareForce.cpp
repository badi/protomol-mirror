#include <protomol/force/SystemCompareForce.h>
#include <protomol/types/ScalarStructure.h>
#include <protomol/types/Vector3DBlock.h>
#include <protomol/topology/GenericTopology.h>
using std::vector;
using std::string;
using namespace ProtoMol::Report;

namespace ProtoMol {
  //________________________________________ SystemCompareForce

  SystemCompareForce::SystemCompareForce(Force *actualForce,
                                         CompareForce *compareForce) :
    CompareForce(actualForce, compareForce) {}
  void SystemCompareForce::evaluate(const GenericTopology *topo,
                                    const Vector3DBlock *positions,
                                    Vector3DBlock *forces,
                                    ScalarStructure *energies) {
    preprocess(positions->size());
    (dynamic_cast<SystemForce *>(myActualForce))->evaluate(topo,
      positions,
      myForces,
      myEnergies);
    postprocess(topo, forces, energies);
  }

  void SystemCompareForce::parallelEvaluate(const GenericTopology *topo,
                                            const Vector3DBlock *positions,
                                            Vector3DBlock *forces,
                                            ScalarStructure *energies) {
    preprocess(positions->size());
    (dynamic_cast<SystemForce *>(myActualForce))->parallelEvaluate(topo,
      positions,
      myForces,
      myEnergies);
    postprocess(topo, forces, energies);
  }
}
