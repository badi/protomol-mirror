#include <protomol/force/ExtendedForce.h>
#include <protomol/force/ForceGroup.h>
#include "ExtendedCompareForce.h"
#include "ExtendedTimeForce.h"

namespace ProtoMol {
  //________________________________________ ExtendedForce

  void ExtendedForce::addToForceGroup(ForceGroup *forceGroup) {
    forceGroup->addExtendedForce(this);
  }

  CompareForce *ExtendedForce::makeCompareForce(Force *actualForce,
                                                CompareForce *compareForce)
  const {
    return new ExtendedCompareForce(actualForce, compareForce);
  }

  TimeForce *ExtendedForce::makeTimeForce(Force *actualForce) const {
    return new ExtendedTimeForce(actualForce);
  }
}
