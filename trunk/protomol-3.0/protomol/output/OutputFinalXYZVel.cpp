#include <protomol/output/OutputFinalXYZVel.h>
#include <protomol/config/Configuration.h>
#include <protomol/output/OutputCache.h>
#include <protomol/util/StringUtilities.h>
#include <protomol/topology/GenericTopology.h>
#include <protomol/io/XYZWriter.h>
#include <protomol/debug/Exception.h>

using namespace std;
using namespace ProtoMol::Report;
using namespace ProtoMol;

//____ OutputFinalXYZVel
const string OutputFinalXYZVel::keyword("finXYZVelFile");

OutputFinalXYZVel::OutputFinalXYZVel() :
  Output(1), myFilename("") {}

OutputFinalXYZVel::OutputFinalXYZVel(const string &filename) :
  Output(1), myFilename(filename) {}

void OutputFinalXYZVel::doFinalize(int step) {
  XYZWriter writer;
  if (!writer.open(myFilename))
    THROW(string("Can't open ") + getId() + " '" + myFilename + "'.");

  writer.setComment("Time : " + toString(cache->time()) + ", step : " +
                    toString(step) + ".");

  if (!writer.write(*myVelocities, myTopology->atoms, myTopology->atomTypes))
    THROW(string("Could not write ") + getId() + " '" + myFilename + "'.");
}

Output *OutputFinalXYZVel::doMake(const vector<Value> &values) const {
  return new OutputFinalXYZVel(values[0]);
}

void OutputFinalXYZVel::getParameters(vector<Parameter> &parameter) const {
  parameter.push_back
    (Parameter(getId(), Value(myFilename, ConstraintValueType::NotEmpty())));
}

