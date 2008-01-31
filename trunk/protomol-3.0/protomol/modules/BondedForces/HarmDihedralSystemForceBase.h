/*  -*- c++ -*-  */
#ifndef HARMDIHEDRALSYSTEMFORCEBASE_H
#define HARMDIHEDRALSYSTEMFORCEBASE_H

#include <string>

namespace ProtoMol {
  //____ HarmDihedralSystemForceBase

  class HarmDihedralSystemForceBase {
  public:
    virtual ~HarmDihedralSystemForceBase() {}

    static const std::string keyword;
  };
}
#endif /* HARMDIHEDRALSYSTEMFORCEBASE_H */

