#include <protomol/output/OutputFinalXYZPos.h>
#include <protomol/config/Configuration.h>
#include <protomol/output/OutputCache.h>
#include <protomol/output/OutputModule.h>
#include <protomol/util/StringUtilities.h>
#include <protomol/topology/GenericTopology.h>
#include <protomol/io/XYZWriter.h>
#include <protomol/debug/Exception.h>

using namespace std;
using namespace ProtoMol::Report;
using namespace ProtoMol;

//____ OutputFinalXYZPos
const string OutputFinalXYZPos::keyword("finXYZPosFile");

OutputFinalXYZPos::OutputFinalXYZPos() :
  Output(1), myFilename(""), myMinimalImage(false) {}

OutputFinalXYZPos::OutputFinalXYZPos(const string &filename, bool minimal) :
  Output(1), myFilename(filename), myMinimalImage(minimal) {}

void OutputFinalXYZPos::doFinalize(int step) {
  XYZWriter writer;
  if (!writer.open(myFilename))
    THROW(string("Can't open ") + getId() + " '" + myFilename + "'.");

  const Vector3DBlock *pos = (myMinimalImage ? cache->minimalPositions() :
                              myPositions);
  writer.setComment("Time : " + toString(cache->time()) + ", step : " +
                    toString(step) +
                    (myMinimalImage ? ", minimal Image" : "") + ".");

  if (!writer.write(*pos, myTopology->atoms, myTopology->atomTypes))
    THROW(string("Could not write ") + getId() + " '" + myFilename + "'.");
}

Output *OutputFinalXYZPos::doMake(const vector<Value> &values) const {
  return new OutputFinalXYZPos(values[0], values[1]);
}

void OutputFinalXYZPos::getParameters(vector<Parameter> &parameter) const {
  parameter.push_back
    (Parameter(getId(), Value(myFilename, ConstraintValueType::NotEmpty())));
  parameter.push_back
    (Parameter(keyword + "MinimalImage", Value(myMinimalImage),
               Text("whether the coordinates should be transformed to minimal"
                    " image or not")));
}

bool OutputFinalXYZPos::adjustWithDefaultParameters(vector<Value> &values,
                                                    const Configuration *config)
const {
  if (!checkParameterTypes(values)) return false;

  if (config->valid(InputMinimalImage::keyword) && !values[1].valid())
    values[1] = (*config)[InputMinimalImage::keyword];

  return checkParameters(values);
}
