#include "PosVelReaderType.h"
using std::string;

namespace ProtoMol {
  //__________________________________________________________PosVelReaderType
  const string PosVelReaderEnum::str[static_cast<int>(LAST) -
                                     static_cast<int>(FIRST)] = {
    // Order is essential, must be in relation to Enum ordering
    string("undefined"), // Returned when no enum matches
    string("PDB"),
    string("XYZ"),
    string("XYZBin")
  };
}
