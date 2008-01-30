#include <protomol/force/SystemForce.h>
#include <protomol/force/ForceGroup.h>
#include <protomol/force/SystemCompareForce.h>
#include <protomol/force/SystemTimeForce.h>

using std::vector;
using std::string;
using namespace ProtoMol::Report;
namespace ProtoMol {
  //________________________________________ SystemForce

  void SystemForce::addToForceGroup(ForceGroup *forceGroup) {
    forceGroup->addSystemForce(this);
  }

  CompareForce *SystemForce::makeCompareForce(Force *actualForce,
                                              CompareForce *compareForce) const
  {
    return new SystemCompareForce(actualForce, compareForce);
  }

  TimeForce *SystemForce::makeTimeForce(Force *actualForce) const {
    return new SystemTimeForce(actualForce);
  }
}
