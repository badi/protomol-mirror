#include <protomol/force/MollyForce.h>
#include <protomol/force/ForceGroup.h>

using std::vector;
using std::string;
using namespace ProtoMol::Report;
namespace ProtoMol {
  //________________________________________ MollyForce

  void MollyForce::addToForceGroup(ForceGroup *forceGroup) {
    forceGroup->addMollyForce(this);
  }
}
