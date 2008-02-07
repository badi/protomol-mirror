/* -*- c++ -*- */
#ifndef MODIFIER_FACTORY_H
#define MODIFIER_FACTORY_H

#include <protomol/base/Factory.h>

#include <protomol/modifier/Modifier.h>
#include <protomol/config/Value.h>

namespace ProtoMol {
  class ModifierFactory : public Factory<Modifier> {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // From Factory<Modifier>
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  public:
    virtual void registerHelpText() const;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // New methods of class ModifierFactory
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  public:
    void registerAllExemplarsConfiguration(Configuration *config) const;
    Modifier *make(const std::string &id, const std::vector<Value> &values)
    const;
  };
}
#endif /* MODIFIER_FACTORY_H */
