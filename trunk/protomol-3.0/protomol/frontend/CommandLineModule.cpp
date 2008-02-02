#include <protomol/frontend/CommandLineModule.h>
#include <protomol/frontend/CommandLine.h>

#include <protomol/frontend/ProtoMolApp.h>

using namespace std;
using namespace ProtoMol;

void CommandLineModule::init(ProtoMolApp *app) {
  CommandLineOption::ActionBase *action;
  CommandLine &cmdLine = app->cmdLine;

  action =
    new CommandLineOption::Action<CommandLine>(&cmdLine,
                                               &CommandLine::usageAction);
  cmdLine.add('h', "help", action, "Print this help screen.");

#ifdef DEBUG
  action =
    new CommandLineOption::
    Action<CommandLine>(&cmdLine, &CommandLine::enableStackTraceAction);

  cmdLine.add('X', "enable-stack-trace", action,
    "Enable stack trace output on exceptions.");
#endif
}

