#include <protomol/io/PosVelReader.h>

#include <protomol/types/Vector.h>
#include <protomol/base/SystemUtilities.h>

#include <protomol/io/XYZBinReader.h>
#include <protomol/io/XYZReader.h>
#include <protomol/io/PDBReader.h>

using namespace std;
using namespace ProtoMol;
//____ File

PosVelReader::PosVelReader() :
  myFilename(""), myOk(true), myType(PosVelReaderType::UNDEFINED) {}

PosVelReader::PosVelReader(const string &filename) :
  myFilename(filename), myOk(isAccessible(filename)),
  myType(PosVelReaderType::UNDEFINED) {}

PosVelReader::operator void*() const {
  return !myOk ? 0 : const_cast<PosVelReader *>(this);
}

bool PosVelReader::operator!() const {
  return !myOk;
}

void PosVelReader::setFilename(const string &filename) {
  myFilename = filename;
  myOk = true;
  myType = PosVelReaderType::UNDEFINED;
}

bool PosVelReader::open() {
  myOk = isAccessible(myFilename);
  return myOk;
}

bool PosVelReader::open(const string &filename) {
  setFilename(filename);
  return open();
}

bool PosVelReader::tryFormat(PosVelReaderType::Enum type) {
  if (type == PosVelReaderType::PDB) {
    PDBReader reader(myFilename);
    return reader.tryFormat();
  } else if (type == PosVelReaderType::XYZ) {
    XYZReader reader(myFilename);
    return reader.tryFormat();
  } else if (type == PosVelReaderType::XYZBIN) {
    XYZBinReader reader(myFilename);
    return reader.tryFormat();
  } else
    return isAccessible(myFilename);
}

PosVelReaderType PosVelReader::getType() const {
  return myType;
}

PosVelReader &ProtoMol::operator>>(PosVelReader &posReader, PDB &pdb) {
  posReader.myType = PosVelReaderType::UNDEFINED;

  // PDB
  PDBReader pdbReader(posReader.myFilename);
  posReader.myOk = pdbReader.tryFormat();
  if (posReader.myOk) {
    posReader.myOk = (pdbReader >> pdb ? true : false);
    posReader.myType = PosVelReaderType::PDB;
  }

  return posReader;
}

PosVelReader &ProtoMol::operator>>(PosVelReader &posReader, XYZ &xyz) {
  posReader.myType = PosVelReaderType::UNDEFINED;

  // XYZ
  XYZReader xyzReader(posReader.myFilename);
  posReader.myOk = xyzReader.tryFormat();
  if (posReader.myOk) {
    posReader.myOk = (xyzReader >> xyz ? true : false);
    posReader.myType = PosVelReaderType::XYZ;
  }
  // PDB
  if (!posReader.myOk) {
    PDBReader pdbReader(posReader.myFilename);
    if (pdbReader.tryFormat()) {
      posReader.myOk = (pdbReader >> xyz ? true : false);
      posReader.myType = PosVelReaderType::PDB;
    }
  }

  return posReader;
}

PosVelReader &ProtoMol::operator>>(PosVelReader &posReader,
                         Vector3DBlock &coords) {
  posReader.myType = PosVelReaderType::UNDEFINED;

  // XYZ
  XYZReader xyzReader(posReader.myFilename);
  posReader.myOk = xyzReader.tryFormat();
  if (posReader.myOk) {
    posReader.myOk = (xyzReader >> coords ? true : false);
    posReader.myType = PosVelReaderType::XYZ;
  }
  // XYZ binary
  if (!posReader.myOk) {
    XYZBinReader xyzBinReader(posReader.myFilename);
    if (xyzBinReader.tryFormat()) {
      posReader.myOk = (xyzBinReader >> coords ? true : false);
      posReader.myType = PosVelReaderType::XYZBIN;
    }
  }

  // PDB
  if (!posReader.myOk) {
    PDBReader pdbReader(posReader.myFilename);
    if (pdbReader.tryFormat()) {
      posReader.myOk = (pdbReader >> coords ? true : false);
      posReader.myType = PosVelReaderType::PDB;
    }
  }

  return posReader;
}

