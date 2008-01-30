/*******************************************************************\

              Copyright (C) 2004 Joseph Coffland

    This program is free software; you can redistribute it and/or
     modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
        of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
             GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
      Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
                           02111-1307, USA.

            For information regarding this software email
                   jcofflan@users.sourceforge.net

\*******************************************************************/

#include "Debugger.h"
#include "Process.h"
#include <protomol/types/String.h>
#include "Exception.h"

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include <sstream>

using namespace std;

string Debugger::executableName;
int Debugger::numTraces = 0;
bool Debugger::leaveCores = false;
bool Debugger::traceFiltering = true;
int Debugger::maxTraces = 10;

void Debugger::initStackTrace(string executableName) {
  Debugger::executableName = executableName;
  Exception::enableStackTraces = true;
}

bool Debugger::printStackTrace(ostream &stream) {
  list<string> trace;
  bool retVal = getStackTrace(trace);

  list<string>::iterator it;
  for (it = trace.begin(); it != trace.end(); it++)
    stream << *it << endl;

  return retVal;
}

#define BUF_SIZE 2048

bool Debugger::_getStackTrace(std::list<std::string> &trace) {
  if (executableName == "") {
    trace.push_back("Stack Trace Error: Stack dumper not initialized!");
    return false;
  }

  numTraces++;
  if (maxTraces && numTraces > maxTraces) {
    trace.push_back("Stack Trace Error: Exceeded maxTraces of " +
                    String(maxTraces));
    return false;
  }

  // Set core ulimit
  struct rlimit coreLimit;
  getrlimit(RLIMIT_CORE, &coreLimit);
  coreLimit.rlim_cur = coreLimit.rlim_max;
  setrlimit(RLIMIT_CORE, &coreLimit);


  int pid = fork();
  
  // Fork child and dump core
  if (pid == 0) {
    abort(); // Dump core
    exit(0);

  } else if (pid == -1) {
    trace.push_back("Stack Trace Error: Creating abort process!");
    return false;
  }

  waitpid(pid, 0, 0);


  // Check for core file
  string coreFilename = "core";
  int coreFD = open(coreFilename.c_str(), O_RDONLY);
  if (coreFD == -1) {
    coreFilename = coreFilename + "." + String(pid);
    coreFD = open(coreFilename.c_str(), O_RDONLY);

    if (coreFD == -1) {
      trace.push_back(string("Stack Trace Error: creating core file!\n") +
                      "This can occur if you do not have write permission\n" +
                      "in the current directory or if the core file limit\n" +
                      "is set to zero.  On many Unix systems the core file\n" +
                      "limit can be set with 'ulimit -c unlimited'.");

      return false;
    }
  }
  close(coreFD);


  // Spawn gdb process
  int argc = 0;
  char *argv[5];

  argv[argc++] = "gdb";
  argv[argc++] = (char *)executableName.c_str();
  argv[argc++] = "-c";
  argv[argc++] = (char *)coreFilename.c_str();
  argv[argc] = 0;

  try {
    Process debugProc;
    Pipe *inPipe = debugProc.getChildPipe(Process::TO_CHILD, 0);
    Pipe *outPipe = debugProc.getChildPipe(Process::FROM_CHILD, 1);
    Pipe *errPipe = debugProc.getChildPipe(Process::FROM_CHILD, 2);
    debugProc.exec(argv);

    // Run gdb commands
    string debugCmd =
      string("set width ") + String(BUF_SIZE - 1) + "\nwhere\nquit\n";
    write(inPipe->getInFD(), debugCmd.c_str(), debugCmd.length());


    // Read output
    FILE *out = fdopen(outPipe->getOutFD(), "r");
    FILE *err = fdopen(errPipe->getOutFD(), "r");
    if (!out || !err) {
      trace.push_back("Stack Trace Error: Opening debugger output streams!");

      return false;
    }

    char buf[BUF_SIZE + 1];
    int offset = 0;
    int count = 0;
    while (fgets(buf, BUF_SIZE, out)) {
      if (buf[0] == '#') {
        
        if (traceFiltering) {
          count++;
      
          if (strstr(buf, "Debugger::") ||
              strstr(buf, "Exception::init") ||
              strstr(buf, "Exception (")) {
            offset = count;
            trace.clear();
            continue;
          }
        }

        int line = atoi(&buf[1]) - offset;
        char *start = strchr(buf, ' ');
        int len = strlen(buf);

        if (buf[len - 1] == '\n' || buf[len - 1] == '\r') buf[len - 1] = 0;
        trace.push_back(string("#") + String(line) + start);
      }
    }

#ifdef DEBUGGER_PRINT_ERROR_STREAM
    while (fgets(buf, BUF_SIZE, err)) {
      int len = strlen(buf);
      if (buf[len - 1] == '\n' || buf[len - 1] == '\r') buf[len - 1] = 0;
      if (buf[0] != 0) trace.push_back(buf);
    }
#endif

    // Clean up
    fclose(out);
    fclose(err);

    if (!leaveCores) unlink(coreFilename.c_str());

    debugProc.wait();
    if (debugProc.getReturnCode()) {
      trace.push_back("Stack Trace Error: gdb returned an error.");

      return false;
    }

    return true;

  } catch (Exception &e) {
    trace.push_back(string("Stack Trace Error: ") + e.getMessage());
  }

  return false;
}

bool Debugger::getStackTrace(std::list<std::string> &trace) {
  static bool inStackTrace = false;
  bool ret;

  if (inStackTrace) {
    trace.push_back("Stack Trace Error: Already in stack trace!");
    return false;    
  }

  inStackTrace = true;

  try {
    ret = _getStackTrace(trace);

  } catch (...) {
    inStackTrace = false;
    throw;
  }
  inStackTrace = false;

  return ret;
}

#ifdef DEBUGGER_TEST
#include <iostream>

void b (int x) {
  THROW("Test cause!");
}

void a(char *x) {
  try {
    b(10);
  } catch (Exception &e) {
    THROWC("Test exception!", e);
  } 
}


int main(int argc, char *argv[]) {
  Debugger::initStackTrace(argv[0]);

  try {
    
    a("test");

  } catch (Exception &e) {
    cerr << "Exception: " << e << endl;
  }

  return 0;
}
#endif // DEBUGGER_TEST
