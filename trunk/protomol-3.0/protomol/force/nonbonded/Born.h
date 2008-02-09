#ifndef BORN_H
#define BORN_H

namespace ProtoMol {
  class TOneAtomPair;
  class GenericTopology;
  class Vector3DBlock;
  class ScalarStructure;

  template<class TOneAtomPair>
  class Born {
  public:
    void evaluateBorn(TOneAtomPair &, GenericTopology *, Vector3DBlock *,
                      ScalarStructure *);
  };
}
#endif
