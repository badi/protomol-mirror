/*  -*- c++ -*-  */
#ifndef NONBONDEDCUTOFFFORCEBASE_H
#define NONBONDEDCUTOFFFORCEBASE_H

#include <string>

namespace ProtoMol {
  //____ NonbondedCutoffForceBase

  class NonbondedCutoffForceBase {
  public:
    virtual ~NonbondedCutoffForceBase() {}

    static const std::string keyword;
  };
}
#endif /* NONBONDEDCUTOFFFORCEBASE_H */

