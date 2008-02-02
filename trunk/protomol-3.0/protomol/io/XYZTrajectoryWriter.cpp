#include <protomol/io/XYZTrajectoryWriter.h>

#include <iomanip>

#include <protomol/base/Report.h>
#include <protomol/base/StringUtilities.h>

using namespace std;
using namespace ProtoMol::Report;
using namespace ProtoMol;

//____XYZTrajectoryWriter

XYZTrajectoryWriter::XYZTrajectoryWriter() :
  Writer(), myCoords(NULL), myNames(NULL), myAtoms(NULL), myAtomTypes(NULL),
  myFirst(true) {}

XYZTrajectoryWriter::XYZTrajectoryWriter(const string &filename) :
  Writer(filename), myCoords(NULL), myNames(NULL), myAtoms(NULL),
  myAtomTypes(NULL), myFirst(true) {}

XYZTrajectoryWriter::~XYZTrajectoryWriter() {}

bool XYZTrajectoryWriter::open(const XYZ &xyz) {
  setNames(xyz.names);
  myFirst = false;
  return open();
}

bool XYZTrajectoryWriter::open(const vector<string> &names) {
  setNames(names);
  myFirst = false;
  return open();
}

bool XYZTrajectoryWriter::open(const vector<Atom> &atoms,
                               const vector<AtomType> &atomTypes) {
  setNames(atoms, atomTypes);
  myFirst = false;
  return open();
}

bool XYZTrajectoryWriter::open(const string &filename, const XYZ &xyz) {
  setNames(xyz.names);
  myFirst = false;
  return open(filename);
}

bool XYZTrajectoryWriter::open(const string &filename,
                               const vector<string> &names) {
  setNames(names);
  myFirst = false;
  return open(filename);
}

bool XYZTrajectoryWriter::open(const string &filename,
                               const vector<Atom> &atoms,
                               const vector<AtomType> &atomTypes) {
  setNames(atoms, atomTypes);
  myFirst = false;
  return open(filename);
}

bool XYZTrajectoryWriter::reopen() {
  if (myFirst) {
    myFirst = false;
    if (!open())
      return false;
  }

  if (myFile.is_open())
    close();

  // Try to read the number of frames
  myFile.clear();
  myFile.open(myFilename.c_str(), ios::in);
  string line, str;
  line = getline();
  close();
  stringstream ss(line);
  ss >> str;
  if (toInt(str) > 0) {
    // Ok, we have already written frames
    str = (toString(toInt(str) + 1) + "                       ").substr(0, 19);
    str += "\n";
    myFile.clear();
    myFile.open(myFilename.c_str(), ios::in | ios::out);
    myFile.seekp(0, ios::beg);
    myFile.write(str.c_str(), str.size());
  } else {
    // First time ...
    myFile.clear();
    myFile.open(myFilename.c_str(), ios::out | ios::trunc);
    str = (toString(1) + "                       ").substr(0, 19);
    str += "\n";
    myFile << str;
  }
  close();
  myFile.clear();
  myFile.open(myFilename.c_str(), ios::out | ios::app);
  return !myFile.fail();
}

bool XYZTrajectoryWriter::write(const XYZ &xyz) {
  setCoords(xyz.coords);
  setNames(xyz.names);
  return write();
}

bool XYZTrajectoryWriter::write(const Vector3DBlock &coords) {
  setCoords(coords);
  return write();
}

bool XYZTrajectoryWriter::write(const Vector3DBlock &coords,
                                const vector<string> &names) {
  setNames(names);
  setCoords(coords);
  return write();
}

bool XYZTrajectoryWriter::write(const Vector3DBlock &coords,
                                const vector<Atom> &atoms,
                                const vector<AtomType> &atomTypes) {
  setNames(atoms, atomTypes);
  setCoords(coords);
  return write();
}

bool XYZTrajectoryWriter::write() {
  if (!reopen())
    return false;
  if (myCoords == NULL)
    report << error << "[XYZTrajectoryWriter::write]"
           << " No coordinates specified." << endr;
  if (!(myNames != NULL || (myAtoms != NULL && myAtomTypes != NULL)))
    report << error << "[XYZTrajectoryWriter::write]"
           << " No atom names specified." << endr;
  const unsigned int count = myCoords->size();
  if ((myNames != NULL && myNames->size() != count) ||
      (myAtoms != NULL && myAtomTypes != NULL && myAtoms->size() != count))
    report << error << "[XYZTrajectoryWriter::write]"
           << " Coorindate and atom name size are not equal." << endr;

  // First, write the number of atoms
  myFile << count << endl;

  // Write atoms
  myFile << setprecision(15);   // This should be some FLT_DIG or DBL_DIG ...
  for (unsigned int i = 0; i < count; ++i) {
    myFile <<
    (myNames !=
     NULL ? (*myNames)[i] : (*myAtomTypes)[(*myAtoms)[i].type].name) << "\t";
    myFile.width(24);
    myFile << (*myCoords)[i].x;
    myFile.width(24);
    myFile << (*myCoords)[i].y;
    myFile.width(24);
    myFile << (*myCoords)[i].z;
    myFile << endl;
  }

  close();
  return !myFile.fail();
}

void XYZTrajectoryWriter::setNames(const XYZ &xyz) {
  myNames = &(xyz.names);
  myAtoms = NULL;
  myAtomTypes = NULL;
}

void XYZTrajectoryWriter::setNames(const vector<string> &names) {
  myNames = &names;
  myAtoms = NULL;
  myAtomTypes = NULL;
}

void XYZTrajectoryWriter::setNames(const vector<Atom> &atoms,
                                   const vector<AtomType> &atomTypes) {
  myNames = NULL;
  myAtoms = &atoms;
  myAtomTypes = &atomTypes;
}

void XYZTrajectoryWriter::setCoords(const Vector3DBlock &coords) {
  myCoords = &coords;
}

XYZTrajectoryWriter &operator<<(XYZTrajectoryWriter &xyzWriter,
                                const XYZ &xyz) {
  xyzWriter.write(xyz);
  return xyzWriter;
}

XYZTrajectoryWriter &operator<<(XYZTrajectoryWriter &xyzWriter,
                                const Vector3DBlock &coords) {
  xyzWriter.write(coords);
  return xyzWriter;
}
