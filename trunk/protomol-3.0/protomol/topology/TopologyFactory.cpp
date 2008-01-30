#include "TopologyFactory.h"

#include <protomol/config/Configuration.h>
#include <protomol/frontend/HelpTextFactory.h>

using namespace std;
using namespace ProtoMol;
using namespace ProtoMol::Report;

//________________________________________ TopologyFactory
void TopologyFactory::
registerAllExemplarsConfiguration(Configuration *config) const {
  for (const_iterator i = begin(); i != end(); ++i) {
    vector<Parameter> parameter = (*i)->getParameters();
    for (unsigned int i = 0; i < parameter.size(); i++)
      config->registerKeyword(parameter[i].keyword, parameter[i].value);
  }

  config->registerKeyword(GenericTopology::getKeyword(),
                          Value(string(""), ConstraintValueType::NotEmpty()));
  cache = false;
}

GenericTopology *TopologyFactory::
make(string &errMsg, const Configuration *config) const {
  string id = config->get(GenericTopology::getKeyword()).getString();
  const GenericTopology *prototype = getPrototype(id);

  return make(errMsg, id,
              config->get(prototype != NULL ? prototype->getParameters() :
                          vector<Parameter>()));
}

GenericTopology *TopologyFactory::
make(string &errMsg, const string &id, const vector<Value> &values) const {
  errMsg = "";
  const GenericTopology *prototype = getPrototype(id);

  if (prototype == NULL) {
    errMsg += " Could not find any match for \'" + id + "\' in " +
      GenericTopology::scope +
      "Factory.\nPossible topologies are:\n" + print();
    return NULL;
  }

  // Make
  GenericTopology *newObj = prototype->make(errMsg, values);
  if (newObj == NULL) return NULL;

  // Adjust external alias
  newObj->setAlias(id);
  return newObj;
}

string TopologyFactory::print() const {
  string res;

  for (exemplars_t::const_iterator i = exemplars.begin(); i != exemplars.end();
       ++i) {
    res += (i == exemplars.begin() ? "" : "\n") + i->first;

    vector<Parameter> parameter(i->second->getParameters());

    for (unsigned int k = 0; k < parameter.size(); k++) {
      if (!parameter[k].keyword.empty())
        res += "\n" + Constant::PRINTINDENT + Constant::PRINTINDENT +
          getRightFill(parameter[k].keyword, Constant::PRINTMAXWIDTH);

      res +=
        (parameter[k].defaultValue.valid() ?
         parameter[k].defaultValue.getDefinitionTypeString() :
         parameter[k].value.getDefinitionTypeString());

      if (!parameter[k].text.empty())
        res += "\t # " + parameter[k].text;
    }
  }

  res += "\nAlias:";
  for (exemplars_t::const_iterator j = aliasExemplars.begin();
       j != aliasExemplars.end(); ++j)
    res += "\n" + j->first + " : " + j->second->getId() + " (" +
      j->second->getIdNoAlias() + ")";

  return res;
}

void TopologyFactory::registerHelpText() const {
  for (exemplars_t::const_iterator i = exemplars.begin();
       i != exemplars.end(); ++i) {
    HelpText helpText;
    i->second->getParameters(helpText.parameters);
    helpText.id = i->second->getIdNoAlias();
    helpText.text = i->second->getText();
    helpText.scope = i->second->getScope();
    HelpTextFactory::registerExemplar(i->second->getId(), helpText);

    HelpText alias;
    alias.text = "alias for \'" + i->second->getId() + "\'";
    alias.scope = i->second->getScope();
    for (exemplars_t::const_iterator j = aliasExemplars.begin();
         j != aliasExemplars.end(); ++j)
      if (j->second->getIdNoAlias() == i->second->getIdNoAlias()) {
        alias.id = j->first;
        HelpTextFactory::registerExemplar(alias.id, alias);
      }
  }
}
