#include <protomol/output/OutputFinalPDBPos.h>
#include <protomol/config/Configuration.h>
#include <protomol/output/OutputCache.h>
#include <protomol/output/OutputModule.h>
#include <protomol/base/StringUtilities.h>
#include <protomol/topology/GenericTopology.h>
#include <protomol/io/PDBWriter.h>
#include <protomol/base/Exception.h>

using namespace std;
using namespace ProtoMol::Report;
using namespace ProtoMol;

//____ OutputFinalPDBPos
const string OutputFinalPDBPos::keyword("finPDBPosFile");

OutputFinalPDBPos::OutputFinalPDBPos() :
  Output(1), filename(""), myMinimalImage(false) {}

OutputFinalPDBPos::OutputFinalPDBPos(const string &filename,
                                     bool minimal) :
  Output(1), filename(filename), myMinimalImage(minimal) {}

void OutputFinalPDBPos::doFinalize(int step) {
  PDBWriter writer;
  if (!writer.open(filename))
    THROW(string("Can't open ") + getId() + " '" + filename + "'.");

  writer.setComment("Time : " + toString(cache->time()) + ", step : " +
                    toString(step) +
                    (myMinimalImage ? ", minimal Image" : "") + ".");

  const Vector3DBlock *pos =
    (myMinimalImage ? cache->minimalPositions() : myPositions);

  if (!writer.write(*pos, cache->pdb()))
    THROW(string("Could not write ") + getId() + " '" + filename + "'.");
}

Output *OutputFinalPDBPos::doMake(const vector<Value> &values) const {
  return new OutputFinalPDBPos(values[0], values[1]);
}

void OutputFinalPDBPos::getParameters(vector<Parameter> &parameter) const {
  parameter.push_back
    (Parameter(getId(), Value(filename, ConstraintValueType::NotEmpty())));
  parameter.push_back
    (Parameter(keyword + "MinimalImage", Value(myMinimalImage),
               Text("whether the coordinates should be transformed to minimal "
                    "image or not")));
}

bool OutputFinalPDBPos::adjustWithDefaultParameters(vector<Value> &values,
                                                    const Configuration *config)
const {
  if (!checkParameterTypes(values)) return false;

  if (config->valid(InputMinimalImage::keyword) && !values[1].valid())
    values[1] = (*config)[InputMinimalImage::keyword];

  return checkParameters(values);
}
