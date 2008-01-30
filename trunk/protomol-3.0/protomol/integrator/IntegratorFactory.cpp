#include "IntegratorFactory.h"

#include <protomol/integrator/STSIntegrator.h>
#include <protomol/integrator/MTSIntegrator.h>
#include <protomol/integrator/NonStandardIntegrator.h>
#include <protomol/util/StringUtilities.h>
#include <protomol/force/ForceFactory.h>
#include <protomol/force/ForceGroup.h>
#include <protomol/util/Report.h>
#include <protomol/frontend/HelpTextFactory.h>

using namespace std;
using namespace ProtoMol;
using namespace ProtoMol::Report;

Integrator *IntegratorFactory::make(const string &definition,
                                    ForceFactory *forceFactory) const {
  string str;
  vector<IntegratorInput> integratorInput;
  stringstream ss(definition);

  while (ss >> str) {
    // Parse level and integrator type
    string levelStr, integrator, d;
    ss >> levelStr >> integrator >> d;
    if (!(equalNocase(str, "level") && isUInt(levelStr) &&
          !integrator.empty() && d == "{"))
      THROW("Integration definition mismatch, expecting \'level <number> "
            "<integrator> { ... }.");

    const Integrator *prototype = getPrototype(integrator);
    if (prototype == NULL)
      THROW(string(
          " Could not find any match for \'") + integrator + "\' in " +
        Integrator::scope + "Factory. Possible integrators are:\n" +
        print());

    // Read first integrator parameters and then force definitions
    string parameterStr, forceStr;
    while (ss >> str) {
      if (str == "}")
        break;
      if (equalNocase(str, "Force") || !forceStr.empty())
        forceStr += (forceStr.empty() ? "" : " ") + str;
      else
        parameterStr += (parameterStr.empty() ? "" : " ") + str;
    }

    parameterStr += " ";  // some compiler need this

    // Expand vector
    unsigned int level = toUInt(levelStr);
    if (integratorInput.size() <= level)
      integratorInput.resize(level + 1);

    // Return if already defined
    if (integratorInput[level].prototype != NULL)
      THROW(" Level " + toString(level) + " already defined with " +
        integratorInput[level].prototype->getId() + ".");

    // Parse integrator parameter
    vector<Parameter> parameters;
    prototype->getParameters(parameters);
    integratorInput[level].values.resize(parameters.size());

    stringstream ssp(parameterStr);
    bool foundLast = false;

    for (unsigned int i = 0; i < parameters.size(); ++i) {
      // parse the the parameters one by one
      string strp;
      integratorInput[level].values[i] = parameters[i].value;
      integratorInput[level].values[i].clear();

      bool found = false;
      bool retry = true;
      if (!(ssp)) {
        ssp.str(parameterStr);
        ssp.seekg(ios::beg);
        ssp.clear();
      }

      while (ssp >> strp || retry) {
        if (!(ssp) && retry) {
          ssp.str(parameterStr);
          ssp.seekg(ios::beg);
          ssp.clear();
          retry = false;
          strp = "";
          continue;
        }
        if (equalNocase(strp,
              parameters[i].keyword) && !parameters[i].keyword.empty()) {
          ssp >> integratorInput[level].values[i];
          found = true;
          break;
        } else if (foundLast && parameters[i].keyword.empty()) {
          ssp.seekg((-1) * static_cast<int>(strp.size()), ios::cur);
          ssp.clear();
          ssp >> integratorInput[level].values[i];
          found = true;
          break;
        }
      }

      foundLast = found;

      // If still undefined take default value is available
      if (!found && parameters[i].defaultValue.valid())
        integratorInput[level].values[i].set(parameters[i].defaultValue);
    }

    string errMsg;
    if (!prototype->checkParameters(errMsg, integratorInput[level].values))
      THROW(string(" Level " + toString(level) + " " + errMsg));

    // Parse forces
    stringstream ssf(forceStr);
    Value force(ValueType::Force(""));
    while (ssf >> force)
      integratorInput[level].forces.push_back(force.getString());

    // Set prototype
    integratorInput[level].prototype = prototype;
  }

  // Check if we have a definition for each level
  bool ok = true;
  string errMsg;
  for (unsigned int i = 0; i < integratorInput.size(); ++i)
    if (integratorInput[i].prototype == NULL) {
      if (ok) errMsg += " Missing integrator definitions of level(s):";
      errMsg += " " + toString(i);
      ok = false;
    }

  if (!ok) THROW(errMsg + ".");

  // Check if the chain is ok
  ok = true;
  for (unsigned int i = 0; i < integratorInput.size(); ++i) {
    const Integrator *prototype = integratorInput[i].prototype;
    if (dynamic_cast<const StandardIntegrator *>(prototype))
      if (!((i == 0 && dynamic_cast<const STSIntegrator *>(prototype)) ||
            i > 0 && dynamic_cast<const MTSIntegrator *>(prototype))) {
        if (i > 0)
          errMsg += " Integrator " + toString(prototype->getId()) +
                    " at level " + toString(i) +
                    " is a STS integrator, expected MTS.";
        else

          errMsg += " Integrator " + toString(prototype->getId()) +
                    " at level " + toString(i) +
                    " is a MTS integrator, expected STS.";
        ok = false;
      } else if (dynamic_cast<const NonStandardIntegrator *>(prototype)) {
        errMsg += " NonStandardIntegrator (level " + toString(i) + " " +
                  toString(prototype->getId()) +
                  ") are not supported by " + Force::scope + "Factory yet.";
        ok = false;
      } else
        THROWS("[IntegratorFactory::make] Found an integrator \'"
               << prototype->getId()
               << "' neither a StandardIntegrator nor NonStandardIntegrator.");
  }

  if (!ok) THROW(errMsg);

  // Now make the integrator chain ... with all forces
  StandardIntegrator *integrator = NULL;
  for (unsigned int i = 0; i < integratorInput.size(); ++i) {
    ForceGroup *forceGroup = new ForceGroup();
    for (unsigned int j = 0; j < integratorInput[i].forces.size(); ++j) {
      Force *force = forceFactory->make(integratorInput[i].forces[j]);
      if (force == NULL) {
        delete forceGroup;
        if (integrator) delete integrator;
        return NULL;
      }
      forceGroup->addForce(force);
    }

    StandardIntegrator *newIntegrator = NULL;
    if (i > 0)
      newIntegrator =
        dynamic_cast<const MTSIntegrator *>(integratorInput[i].prototype)->
          make(integratorInput[i].values, forceGroup, integrator);
    else
      newIntegrator =
        dynamic_cast<const STSIntegrator *>(integratorInput[i].prototype)->
          make(integratorInput[i].values, forceGroup);

    if (newIntegrator == NULL) {
      delete forceGroup;
      if (integrator) delete integrator;
      return NULL;
    }
    integrator = newIntegrator;
  }

  return integrator;
}

string IntegratorFactory::print() const {
  string res;
  for (map<string, const Integrator *,
           ltstrNocase>::const_iterator i = exemplars.begin();
       i != exemplars.end();
       ++i) {
    res += (i == exemplars.begin() ? "" : "\n") + i->first;
    vector<Parameter> parameter(i->second->getParameters());
    for (unsigned int k = 0; k < parameter.size(); k++) {
      if (!parameter[k].keyword.empty())
        res += "\n" + Constant::PRINTINDENT + Constant::PRINTINDENT +
               getRightFill(parameter[k].keyword, Constant::PRINTMAXWIDTH);
      res += " " +
             (parameter[k].defaultValue.valid() ? parameter[k].defaultValue.
                getDefinitionTypeString() :
              parameter[k].value.
                getDefinitionTypeString());
      if (!parameter[k].text.empty())
        res += "\t # " + parameter[k].text;
    }
  }

  res += "\nAlias:";
  for (map<string, const Integrator *,
           ltstrNocase>::const_iterator j = aliasExemplars.begin();
       j != aliasExemplars.end();
       ++j)
    res += "\n" + j->first + " : " + j->second->getId() + " (" +
           j->second->getIdNoAlias() + ")";

  return res;
}

void IntegratorFactory::registerHelpText() const {
  for (map<string, const Integrator *,
           ltstrNocase>::const_iterator i = exemplars.begin();
       i != exemplars.end();
       ++i) {
    HelpText helpText;
    i->second->getParameters(helpText.parameters);
    helpText.id = i->second->getIdNoAlias();
    helpText.text = i->second->getText();
    helpText.scope = i->second->getScope();
    HelpTextFactory::registerExemplar(i->second->getId(), helpText);

    HelpText alias;
    alias.text = "alias for \'" + i->second->getId() + "\'";
    alias.scope = i->second->getScope();
    for (map<string, const Integrator *,
             ltstrNocase>::const_iterator j = aliasExemplars.begin();
         j != aliasExemplars.end();
         ++j)
      if (j->second->getIdNoAlias() == i->second->getIdNoAlias()) {
        alias.id = j->first;
        HelpTextFactory::registerExemplar(alias.id, alias);
      }
  }
}

