#include <protomol/output/OutputFAHFile.h>
#include <protomol/ProtoMolApp.h>
#include <protomol/module/MainModule.h>
#include <protomol/module/OutputModule.h>

#include <sstream>

using namespace std;
using namespace ProtoMol::Report;
using namespace ProtoMol;

//____ OutputFAHFile
const string OutputFAHFile::keyword("FAHFile");

OutputFAHFile::OutputFAHFile() {}

OutputFAHFile::OutputFAHFile(const string &filename, int freq) :
  Output(freq), filename(filename) {}

void OutputFAHFile::doInitialize() {}

void OutputFAHFile::doRun(int step) {
  open(filename.c_str(), ios::out | ios::trunc);

  if (is_open()) {
    stringstream str;
    str << app->positions.size() << '\t' << "Alanine" << endl;
    file.write(str.str().c_str(), str.str().length());

    for (unsigned int i = 0; i < app->positions.size(); i++) {
      str.clear();

      str << i + 1 << '\t'
          << app->topology->atomTypes[app->topology->atoms[i].type].name
          << '\t'
          << app->positions[i].x << '\t'
          << app->positions[i].y << '\t'
          << app->positions[i].z << '\t' << 1
          << endl;

      file.write(str.str().c_str(), str.str().length());
    }
    
    close();
  }
}

void OutputFAHFile::doFinalize(int step) {}

Output *OutputFAHFile::doMake(const vector<Value> &values) const {
  return new OutputFAHFile(values[0], values[1]);
}

bool OutputFAHFile::isIdDefined(const Configuration *config) const {
  return config->valid("outputFreq") && !config->empty(getId()) &&
    (!config->valid(getId()) || ((*config)[getId()] == true));
}

void OutputFAHFile::getParameters(vector<Parameter> &parameter) const {
  parameter.push_back
    (Parameter(keyword + "filename",
               Value(filename, ConstraintValueType::NotEmpty())));
  parameter.push_back
    (Parameter(keyword + "OutputFreq",
               Value(myOutputFreq, ConstraintValueType::Positive())));
}

bool OutputFAHFile::adjustWithDefaultParameters(vector<Value> &values,
                                               const Configuration *config)
const {
  if (!checkParameterTypes(values)) return false;

  if (config->valid(InputOutputfreq::keyword) && !values[0].valid())
    values[0] = (*config)[InputOutputfreq::keyword];

  return checkParameters(values);
}
