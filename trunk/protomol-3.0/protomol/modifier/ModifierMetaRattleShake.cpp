#include <protomol/modifier/ModifierMetaRattleShake.h>
#include <protomol/topology/Topology.h>
#include <protomol/topology/TopologyUtilities.h>

using namespace std;
using namespace ProtoMol;

//____ ModifierMetaRattleShake
ModifierMetaRattleShake::ModifierMetaRattleShake(Real eps, int maxIter,
                                                 int order) :
  Modifier(order), myEpsilon(eps), myMaxIter(maxIter), myListOfConstraints(0) {}

void ModifierMetaRattleShake::doInitialize() {
  myLastPositions = (*myPositions);

  // ... maybe it's a second inittialize, add the old back.
  myTopology->degreesOfFreedom += myTopology->bondRattleShakeConstraints.size();

  buildRattleShakeBondConstraintList(myTopology,
                                     myTopology->bondRattleShakeConstraints);
  // this list contains bonded pairs, and UB-bonded pairs excluding
  // (heavy atom)-H pairs and (heavy)-(heavy) pairs

  // subtract the # of constraints from the # of degrees of freedom of the 
  // system. This is needed so that we get the correct temperature
  myTopology->degreesOfFreedom -= myTopology->bondRattleShakeConstraints.size();

  myListOfConstraints = &(myTopology->bondRattleShakeConstraints);
}
