/* -*- c++ -*- */
#ifndef OUTPUT_FACTORY_H
#define OUTPUT_FACTORY_H

#include <protomol/util/Factory.h>

#include <protomol/output/Output.h>
#include <protomol/config/Value.h>
namespace ProtoMol {
  class OutputCollection;

  //____ OutputFactory
  class OutputFactory : public Factory<Output> {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // From Factory<Output>
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  public:
    virtual std::string print() const;
    virtual void registerHelpText() const;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // New methods of class DetailsOutputFactory
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  public:
    void registerAllExemplarsConfiguration(Configuration *config) const;
    Output *make(const std::string &id, const std::vector<Value> &values) const;
    OutputCollection *makeCollection(const Configuration *config) const;
  };
}
#endif /* OUTPUT_FACTORY_H */
