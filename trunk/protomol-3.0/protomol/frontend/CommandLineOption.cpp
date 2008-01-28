#include "CommandLineOption.h"

#include <ctype.h>

using namespace ProtoMol;
using namespace std;

CommandLineOption::CommandLineOption(const char shortName,
                                     const std::string longName,
                                     CommandLineOption::ActionBase *action,
                                     const std::string help) :
  action(action), help(help) {
  if (shortName) addShortAlias(shortName);
  if (longName != "") addLongAlias(longName);
}

CommandLineOption::~CommandLineOption() {
  if (action) delete action;
}

bool CommandLineOption::match(const std::string &arg) const {
  return names.find(arg) != names.end();
}
