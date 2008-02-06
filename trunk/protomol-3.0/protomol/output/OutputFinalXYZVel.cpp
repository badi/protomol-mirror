#include <protomol/output/OutputFinalXYZVel.h>
#include <protomol/config/Configuration.h>
#include <protomol/output/OutputCache.h>
#include <protomol/base/StringUtilities.h>
#include <protomol/topology/GenericTopology.h>
#include <protomol/io/XYZWriter.h>
#include <protomol/base/Exception.h>

using namespace std;
using namespace ProtoMol::Report;
using namespace ProtoMol;

//____ OutputFinalXYZVel
const string OutputFinalXYZVel::keyword("finXYZVelFile");

OutputFinalXYZVel::OutputFinalXYZVel() :
  Output(1), filename("") {}

OutputFinalXYZVel::OutputFinalXYZVel(const string &filename) :
  Output(1), filename(filename) {}

void OutputFinalXYZVel::doFinalize(int step) {
  XYZWriter writer;
  if (!writer.open(filename))
    THROW(string("Can't open ") + getId() + " '" + filename + "'.");

  writer.setComment("Time : " + toString(cache->time()) + ", step : " +
                    toString(step) + ".");

  if (!writer.write(*myVelocities, myTopology->atoms, myTopology->atomTypes))
    THROW(string("Could not write ") + getId() + " '" + filename + "'.");
}

Output *OutputFinalXYZVel::doMake(const vector<Value> &values) const {
  return new OutputFinalXYZVel(values[0]);
}

void OutputFinalXYZVel::getParameters(vector<Parameter> &parameter) const {
  parameter.push_back
    (Parameter(getId(), Value(filename, ConstraintValueType::NotEmpty())));
}

