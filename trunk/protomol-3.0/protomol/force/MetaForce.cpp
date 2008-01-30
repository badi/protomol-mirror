#include <protomol/force/MetaForce.h>
#include <protomol/force/ForceGroup.h>

using std::vector;
using std::string;
using namespace ProtoMol::Report;
namespace ProtoMol {
  //________________________________________ MetaForce

  void MetaForce::addToForceGroup(ForceGroup *forceGroup) {
    forceGroup->addMetaForce(this);
  }
}
