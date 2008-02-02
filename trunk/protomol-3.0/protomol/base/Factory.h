/* -*- c++ -*- */
#ifndef FACTORY_H
#define FACTORY_H

#include <protomol/util/StringUtilities.h>
#include <protomol/util/Report.h>
#include <protomol/debug/Exception.h>

#include <vector>
#include <map>
#include <set>

namespace ProtoMol {
  //________________________________________ Factory
  /**
     Base class of all factories templated with the family type.
     Container to keep pointers for each prototype exemplar and their
     aliases, where the real prototype is keep in a separate set.
   */
  template<typename Type>
  class Factory {
  public:
    typedef std::map<std::string, const Type *, ltstrNocase> exemplars_t;
    typedef std::set<const Type *> pointers_t;

  protected:
    exemplars_t exemplars;
    exemplars_t aliasExemplars;
    pointers_t pointers;
    mutable bool cache;

  public:
    typedef typename pointers_t::const_iterator const_iterator;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Constructors, destructors, assignment
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Factory() : cache(false) {}
    virtual ~Factory() {unregisterAllExemplars();}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // New methods of class Factory
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  public:
    void registerExemplar(const Type *exemplar, std::string id = "") {
      if (id.empty()) id = exemplar->getId();

      if (exemplars.find(id) != exemplars.end())
        Report::report
          << Report::hint << "Prototype '" << id << "' already registered in "
          << Type::scope << "Factory" << Type::scope
          << ", overwriting." << Report::endr;

      if (exemplar->getParameterSize() != exemplar->getParameters().size())
        THROWS(Type::scope << "Factory" << Type::scope << " prototype '" << id
               << "' has different parameter size definitions "
               << exemplar->getParameterSize() << " != "
               << exemplar->getParameters().size() << ", fix it.");

      exemplars[id] = exemplar;
      pointers.insert(exemplar);
      cache = false;
    }

    void registerExemplar(const Type *exemplar,
                          const std::vector<std::string> &aliases,
                          std::string id = "") {
      if (id.empty()) id = exemplar->getId();

      registerExemplar(exemplar, id);
      for (unsigned int i = 0; i < aliases.size(); i++)
        aliasExemplars[aliases[i]] = exemplar;

      cache = false;
    }

    bool unregisterExemplar(const std::string &id) {
      // Get object pointer
      const Type *p = getPrototype(id);
      if (p == NULL) return false;

      // Remove pointers
      for (typename exemplars_t::iterator i = exemplars.begin();
           i != exemplars.end(); i++)
        if (i->second == p) exemplars.erase(i);

      for (typename exemplars_t::iterator i = aliasExemplars.begin();
           i != aliasExemplars.end(); i++)
        if (i->second == p) aliasExemplars.erase(i);

      pointers.erase(p);

      // ... and delete the object
      delete p;

      cache = false;
      return true;
    }

    void unregisterAllExemplars() {
      for (typename pointers_t::iterator i = pointers.begin();
           i != pointers.end(); ++i)
        delete (*i);

      exemplars.clear();
      aliasExemplars.clear();
      pointers.clear();
      cache = false;
    }

    virtual std::string print() const = 0;
    virtual void registerHelpText() const = 0;

    bool empty() const {return pointers.empty();}
    const_iterator begin() const {return pointers.begin();}
    const_iterator end() const {return pointers.end();}
    const Type *find(const std::string &id) const {return getPrototype(id);}

  protected:
    const Type *getPrototype(const std::string &id) const {
      const Type *prototype = NULL;

      if (exemplars.find(id) != exemplars.end())
        prototype = exemplars.find(id)->second;
      else if (aliasExemplars.find(id) != aliasExemplars.end())
        prototype = aliasExemplars.find(id)->second;

      return prototype;
    }
  };
}
#endif /* FACTORY_H */
