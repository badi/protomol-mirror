#include <protomol/module/CommandLineModule.h>
#include <protomol/config/CommandLine.h>

#include <protomol/ProtoMolApp.h>

using namespace std;
using namespace ProtoMol;

void CommandLineModule::init(ProtoMolApp *app) {
  CommandLineOption::ActionBase *action;
  CommandLine &cmdLine = app->cmdLine;

  action =
    new CommandLineOption::Action<CommandLine>(&cmdLine,
                                               &CommandLine::usageAction);
  cmdLine.add('h', "help", action, "Print this help screen.");

  action =
    new CommandLineOption::Action<CommandLine>(&cmdLine,
                                               &CommandLine::splashAction);
  cmdLine.add(0, "splash", action, "Print splash screen and exit.");

#ifdef DEBUG
  action =
    new CommandLineOption::
    Action<CommandLine>(&cmdLine, &CommandLine::enableStackTraceAction);

  cmdLine.add('X', "enable-stack-trace", action,
    "Enable stack trace output on exceptions.");
#endif
}

