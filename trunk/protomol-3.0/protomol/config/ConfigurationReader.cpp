#include <protomol/config/ConfigurationReader.h>

#include <protomol/util/Report.h>
#include <protomol/util/StringUtilities.h>

using namespace std; 
using namespace ProtoMol::Report;
using namespace ProtoMol;
//____ConfigurationReader

ConfigurationReader::ConfigurationReader() :
  Reader(), myConfig(NULL) {}

ConfigurationReader::ConfigurationReader(const string &filename) :
  Reader(filename), myConfig(NULL) {}

ConfigurationReader::~ConfigurationReader() {
  if (myConfig != NULL)
    delete myConfig;
}

bool ConfigurationReader::tryFormat() {
  open();
  return !myFile.fail();
}

bool ConfigurationReader::read() {
  if (myConfig == NULL)
    myConfig = new Configuration();
  return read(*myConfig);
}

bool ConfigurationReader::read(Configuration &config) {
  if (!tryFormat())
    return false;
  if (!open())
    return false;

  // Remove comments and reformat
  stringstream all;
  while (!myFile.eof() && !myFile.fail()) {
    string line(getline());
    stringstream ss(string(line.begin(), find(line.begin(),
                        line.end(), '#')));
    string str;
    while (ss >> str)
      all << (all.str().empty() ? "" : " ") << str;
  }

  close();
  if (myFile.fail())
    return false;

  // Nothing to do ...
  if (all.str().empty())
    return true;

  // First get the keyword and then let Value read from istream ...
  string str;
  string bad;
  bool res = true;
  while (all >> str)
    if (!config.empty(str)) {
      if (!bad.empty()) {
        report << recoverable << "Ignoring:" << bad << endr;
        bad = "";
      }
      ios::pos_type start = all.tellg();
      all >> config[str];
      if (!config[str].valid()) {
        ios::pos_type end = all.tellg();
        if (end > start) {
          streamsize len = static_cast<streamsize>(end - start);
          string tmp(len, ' ');
          all.seekg(start);
          all.read(&(tmp[0]), len);
          report << recoverable << "Could not parse \'" <<
          removeBeginEndBlanks(tmp) << "\' for keyword \'" << str
                 << "\', expecting type " <<
          config[str].getDefinitionTypeString() << "." << endr;
        }
      }
    } else {
      res = false;
      bad += " " + str;
    }

  if (!bad.empty())
    report << recoverable << "Ignoring:" << bad << endr;
  return res;
}

Configuration *ConfigurationReader::orphanConfiguration() {
  Configuration *tmp = myConfig;
  myConfig = NULL;
  return tmp;
}

ConfigurationReader &ProtoMol::operator>>(ConfigurationReader &configReader,
                                          Configuration &config) {
  configReader.read(config);
  return configReader;
}

