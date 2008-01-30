#include <protomol/force/TimeForce.h>
#include <protomol/types/ScalarStructure.h>
#include <protomol/util/MathUtilities.h>
#include <protomol/types/Vector3DBlock.h>
#include <protomol/topology/GenericTopology.h>

#ifdef HAVE_PARALLEL
#include "Parallel.h"
#endif

using std::string;
using std::vector;
using namespace ProtoMol::Report;

namespace ProtoMol {
  //________________________________________ TimeForce

  const string TimeForce::keyword("time");
  unsigned int TimeForce::myCounter = 0;

  TimeForce::TimeForce(Force *actualForce) {
    if (actualForce == NULL)
      report << error << "Actual force is a zero pointer." << endr;
    myActualForce = actualForce;
    myForcename = actualForce->getKeyword() + "." + toString(myCounter);
    myIdNumber = myCounter;
    myCounter++;
    myTimer.reset();
    myTimeList.resize(0);
  }

  TimeForce::~TimeForce() {
    if (myActualForce != NULL) delete myActualForce;
    double totalTime = myTimer.getTime().getRealTime();
    for (int i = myTimeList.size() - 1; i > 0; i--)
      myTimeList[i] = myTimeList[i] - myTimeList[i - 1];

    report.setf(std::ios::showpoint | std::ios::fixed);
    report.precision(5);
    report << allnodesserial << plain
           << "Timing " << myIdNumber
#ifdef HAVE_PARALLEL
           << (Parallel::isParallel() ? string(" (" +
                 toString(Parallel::getId()) + ")") : string(""))
#endif
           << " : " << totalTime << "[s] process time";
    if (myTimeList.size() > 1) {
      double stddev = 0;
      double average = totalTime / ((double)myTimeList.size());
      for (unsigned int i = 0; i < myTimeList.size(); i++)
        stddev += power<2>(myTimeList[i].getRealTime() - average);

      stddev = sqrt(stddev / ((double)myTimeList.size() - 1.0));
      report << ", " << average << "[s] average, "
             << "standard deviation " << stddev
             << ", n=" << myTimeList.size();
    }
    report << "." << endr;
    report.reset();
  }

  void TimeForce::preprocess(unsigned int) {
    myTimer.start();
  }

  void TimeForce::postprocess(const GenericTopology *,
                              Vector3DBlock *,
                              ScalarStructure *) {
    // Timer
    myTimer.stop();
    myTimeList.push_back(myTimer.getTime());
  }

  void TimeForce::getParameters(vector<Parameter> &parameters) const {
    myActualForce->getParameters(parameters);
  }

  unsigned int TimeForce::getParameterSize() const {
    return myActualForce->getParameterSize();
  }

  string TimeForce::getIdNoAlias() const {
    return string(TimeForce::keyword + " " + myActualForce->getIdNoAlias());
  }

  Force *TimeForce::doMake(string &errMsg, vector<Value> values) const {
    return myActualForce->make(errMsg, values);
  }

  unsigned int TimeForce::numberOfBlocks(const GenericTopology *topo,
                                         const Vector3DBlock *positions) {
    return myActualForce->numberOfBlocks(topo, positions);
  }

  void TimeForce::uncache() {
    myActualForce->uncache();
  }

  void TimeForce::doSetParameters(std::string &errMsg,
                                  std::vector<Value> values) {
    myActualForce->setParameters(errMsg, values);
  }
}
