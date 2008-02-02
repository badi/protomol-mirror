#include <protomol/base/ProtoMolApp.h>
#include <protomol/base/Exception.h>

#include <iostream>

using namespace std;
using namespace ProtoMol;

extern void moduleInitFunction(ModuleManager *);

int main(int argc, char *argv[]) {
  try {
    ModuleManager modManager;
    moduleInitFunction(&modManager);

    ProtoMolApp app(&modManager);

    if (!app.configure(argc, argv)) return 0;

    app.read();
    app.build();

    while (app.step())
      cout << "Step: " << app.currentStep << endl;

    app.finalize();

    return 0;
  } catch (const Exception &e) {
    cerr << "ERROR: " << e.getMessage() << endl;
#ifdef DEBUG
    cerr << e << endl;
#endif
  }

  return 1;
}

