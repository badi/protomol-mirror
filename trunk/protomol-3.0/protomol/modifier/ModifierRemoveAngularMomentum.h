/*  -*- c++ -*-  */
#ifndef MODIFIERREMOVEANGULARMOMENTUM_H
#define MODIFIERREMOVEANGULARMOMENTUM_H

#include <protomol/modifier/Modifier.h>
#include <protomol/topology/TopologyUtilities.h>
#include <protomol/base/ProtoMolApp.h>

namespace ProtoMol {
  //____ ModifierRemoveAngularMomentum
  class ModifierRemoveAngularMomentum : public Modifier {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Constructors, destructors, assignment
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  public:
    ModifierRemoveAngularMomentum(int freq) : Modifier(Constant::MAX_INT - 200),
      myStep(0), myFreq(freq) {}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // From class Makeable
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual void getParameters(std::vector<Parameter> &parameters) const {}
    virtual std::string getIdNoAlias() const {return "RemoveAngularMomentum";}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // From class Modifier
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  public:
    virtual bool isInternal() const {return false;}

  private:
    virtual void doExecute() {
      if (myFreq == 0 || 0 == (myStep = (myStep % myFreq)))
        removeAngularMomentum(&app->positions, &app->velocities, app->topology);
      myStep++;
    }

  private:
    virtual void doInitialize() {myStep = 0;}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // My data members
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  private:
    int myStep;
    int myFreq;
  };
}
#endif /* MODIFIER_H */
