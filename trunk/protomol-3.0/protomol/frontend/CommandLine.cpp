#include <protomol/frontend/CommandLine.h>

#include <protomol/debug/Exception.h>
#include <protomol/types/String.h>
#include <protomol/config/Configuration.h>
#include <protomol/config/ConfigurationModule.h>
#include <protomol/util/StringUtilities.h>
#include <protomol/util/SystemUtilities.h>

#include <stdlib.h>

using namespace std;
using namespace ProtoMol;

CommandLine::CommandLine(Configuration *config) :
  config(config), name("ProtoMol") {}

void CommandLine::add(CommandLineOption *option) {
  if (!option) THROW("option is NULL");

  CommandLineOption::names_t::iterator it;
  for (it = option->names.begin(); it != option->names.end(); it++) {
    if (optionMap.find(*it) != optionMap.end())
      THROW("option '" + *it + "' already exists");

    optionMap[*it] = option;
  }

  options.push_back(option);
}

CommandLineOption *CommandLine::add(const char shortName, const string longName,
                                    CommandLineOption::ActionBase *action,
                                    const string help) {
  CommandLineOption *option =
    new CommandLineOption(shortName, longName, action, help);
  add(option);

  return option;
}

int CommandLine::parse(int argc, char *argv[]) {
  name = argv[0];
  bool configSet = false;

  for (int i = 1; i < argc;) {
    string optionStr = argv[i];
    CommandLineOption *option = optionMap[optionStr];

    if (option) {
      vector<string> args;

      // Self arg
      args.push_back(argv[i++]);

      // Required args
      for (unsigned int j = 0; j < option->requiredArgs.size(); j++) {
        if (argc <= i)
          THROW(string("Missing required argument for option ") + optionStr);

        args.push_back(argv[i++]);
      }

      // Optional args
      for (unsigned int j = 0; j < option->optionalArgs.size(); j++) {
        if (argc <= i || argv[i][0] == '-') break;

        args.push_back(argv[i++]);
      }

      // Call action
      if (option->action) {
        int ret = (*option->action)(args);
        if (ret == -1) return -1;
      }
    } else if (argv[i][0] == '-' && argv[i][1] == '-') {
      string key = &argv[i++][2];

      const Configuration *const_config = config;
      Configuration::const_iterator it = const_config->find(key);
      if (it == const_config->end())
        THROW(string("Invalid keyword '") + key + "'");

      if (it->second.getType() == ValueType::VECTOR ||
          it->second.getType() == ValueType::VECTOR3D)
        THROW(string("Keyword '") + key +
          "' with Vector type cannot be set from the command line.");

      if (i == argc)
        THROW(string("Missing argument for keyword '") + key + "'.");
      string val = argv[i++];

      if (!config->set(key, val))
        THROW(string("Invalid value '") + val + "' for keyword '" + key + "'.");
    } else if (!configSet) {
      vector<string> args;

      args.push_back("--config");
      args.push_back(argv[i++]);

      (*optionMap["--config"]->action)(args);
      configSet = true;
    } else THROW(string("Invalid argument '") + argv[i] + "'");
  }

  return 0;
}

int CommandLine::usageAction(const vector<string> &args) {
  usage(cerr, name);
  return -1;
}

void CommandLine::usage(ostream &stream, const string &name) {
  stream
    << "Usage: " << name << " [--config] <filename> [--option args]..." << endl
    << "Options:" << endl;

  for (unsigned int i = 0; i < options.size(); i++) {
    int count = 2;
    stream << "  ";

    // Short option names
    bool first = true;
    for (CommandLineOption::names_t::iterator it = options[i]->names.begin();
         it != options[i]->names.end(); it++) {
      if ((*it)[0] == '-' && (*it)[1] == '-') continue;

      if (first) first = false;
      else {
        stream << "|";
        count++;
      }

      stream << *it;
      count += (*it).length();
    }

    // Long option names
    for (CommandLineOption::names_t::iterator it = options[i]->names.begin();
         it != options[i]->names.end(); it++) {
      if ((*it)[0] == '-' && (*it)[1] != '-') continue;

      if (first) first = false;
      else {
        stream << "|";
        count++;
      }

      stream << *it;
      count += (*it).length();
    }

    // Args
    for (unsigned int j = 0; j < options[i]->requiredArgs.size(); j++) {
      stream << " <" << options[i]->requiredArgs[j] << ">";
      count += 3 + options[i]->requiredArgs[j].length();
    }

    for (unsigned int j = 0; j < options[i]->optionalArgs.size(); j++) {
      stream << " [" << options[i]->optionalArgs[j] << "]";
      count += 3 + options[i]->optionalArgs[j].length();
    }

    // Help
    if (count >= 40) {
      count = 0;
      stream << endl;
    }

    fillFormat(stream, options[i]->help, count, 40);
  }
}

#ifdef DEBUG
int CommandLine::enableStackTraceAction(const vector<string> &args) {
  Debugger::initStackTrace(getCanonicalPath(name));
  Exception::enableStackTraces = true;

  return 0;
}

#endif
