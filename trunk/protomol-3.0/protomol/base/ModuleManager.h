#ifndef MODULE_MANAGER_H
#define MODULE_MANAGER_H

#include <string>
#include <set>
#include <map>

#include "Module.h"

namespace ProtoMol {
  class ProtoMolApp;

  class ModuleManager {
    typedef std::map<std::string, Module *> nameMap_t;
    nameMap_t nameMap;

    typedef std::set<Module *, moduleLess> modules_t;
    modules_t modules;

  public:
    void add(Module *m);
    void remove(Module *m);
    Module *find(const std::string &name);

    void init(ProtoMolApp *app);
    void configure(ProtoMolApp *app);
    void registerForces(ProtoMolApp *app);

    int listAction();
  };
};

#endif // MODULE_MANAGER_H
