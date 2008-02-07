#include <protomol/modifier/ModifierFactory.h>

using namespace std;
using namespace ProtoMol;

void ModifierFactory::registerHelpText() const {
}

void ModifierFactory::
registerAllExemplarsConfiguration(Configuration *config) const {
}

Modifier *ModifierFactory::
make(const string &id, const vector<Value> &values) const {
  return 0;
}
