#include <protomol/io/DCDTrajectoryWriter.h>

#include <protomol/base/Report.h>
#include <protomol/base/SystemUtilities.h>
#include <protomol/base/StringUtilities.h>

using namespace std;
using namespace ProtoMol::Report;
using namespace ProtoMol;

//____DCDTrajectoryWriter

DCDTrajectoryWriter::DCDTrajectoryWriter(Real timestep,
                                         unsigned int firststep,
                                         bool isLittleEndian) :
  Writer(ios::binary | ios::trunc), myIsLittleEndian(isLittleEndian),
  myFirstStep(firststep), myTimeStep(timestep), myFirst(true)
{}

DCDTrajectoryWriter::DCDTrajectoryWriter(const string &filename,
                                         Real timestep,
                                         unsigned int firststep,
                                         bool isLittleEndian) :
  Writer(ios::binary | ios::trunc,
         filename), myIsLittleEndian(isLittleEndian), myFirstStep(firststep),
  myTimeStep(timestep), myFirst(true)
{}

DCDTrajectoryWriter::DCDTrajectoryWriter(const char *filename, Real timestep,
                                         unsigned int firststep,
                                         bool isLittleEndian) :
  Writer(ios::binary | ios::trunc,
         string(filename)), myIsLittleEndian(isLittleEndian),
  myFirstStep(firststep), myTimeStep(timestep), myFirst(true)
{}

bool DCDTrajectoryWriter::open(Real timestep, unsigned int firststep,
                               bool isLittleEndian) {
  setTimestep(timestep);
  setFirststep(firststep);
  setLittleEndian(isLittleEndian);
  myFirst = false;
  return open();
}

bool DCDTrajectoryWriter::open(const string &filename, Real timestep,
                               unsigned int firststep,
                               bool isLittleEndian) {
  setTimestep(timestep);
  setFirststep(firststep);
  setLittleEndian(isLittleEndian);
  myFirst = false;
  return open(filename);
}

bool DCDTrajectoryWriter::open(const char *filename, Real timestep,
                               unsigned int firststep,
                               bool isLittleEndian) {
  setTimestep(timestep);
  setFirststep(firststep);
  setLittleEndian(isLittleEndian);
  myFirst = false;
  return open(string(filename));
}

bool DCDTrajectoryWriter::reopen(unsigned int numAtoms) {
  if (myFirst)
    open();
  myFirst = false;
  if (myFile.is_open())
    close();

  // Try to read the number of frames
  myFile.clear();
  myFile.open(myFilename.c_str(), ios::binary | ios::in);
  myFile.seekg(0, ios::end);
  ios::pos_type size = myFile.tellg();
  close();
  if (myFile.fail())
    return false;

  int32 nAtoms = static_cast<int32>(numAtoms);
  int32 numSets = 1;
  int32 numSteps = 1;
  int32 firstStep = static_cast<int32>(myFirstStep);
  float4 timeStep = static_cast<float4>(myTimeStep) *
                    Constant::INV_TIMEFACTOR;

  int32 n0 = 0;
  int32 n2 = 2;
  int32 n4 = 4;
  int32 n24 = 24;
  int32 n84 = 84;
  int32 n164 = 164;

  if (myIsLittleEndian != ISLITTLEENDIAN) {
    swapBytes(nAtoms);
    swapBytes(numSets);
    swapBytes(numSteps);
    swapBytes(firstStep);
    swapBytes(timeStep);

    swapBytes(n0);
    swapBytes(n2);
    swapBytes(n4);
    swapBytes(n24);
    swapBytes(n84);
    swapBytes(n164);
  }

  if (size > static_cast<ios::pos_type>(100)) {
    // Ok, we have already written frames
    myFile.clear();
    myFile.open(myFilename.c_str(), ios::binary | ios::in);
    myFile.seekg(8, ios::beg);
    File::read(reinterpret_cast<char *>(&numSets), 4);
    close();

    if (myIsLittleEndian != ISLITTLEENDIAN)
      swapBytes(numSets);
    ++numSets;
    if (myIsLittleEndian != ISLITTLEENDIAN)
      swapBytes(numSets);

    myFile.clear();
    myFile.open(
      myFilename.c_str(), ios::binary | ios::in | ios::out);
    myFile.seekp(8, ios::beg);
    //  8: Number of sets of coordinates, NAMD=0 ???
    myFile.write(reinterpret_cast<char *>(&numSets), 4);  
    myFile.seekp(20, ios::beg);
    // 20: Number of sets of coordinates, NAMD=0 ???
    myFile.write(reinterpret_cast<char *>(&numSets), 4);   
    close();
  } else {
    // First time ...
    myFile.clear();
    myFile.open(
      myFilename.c_str(), ios::binary | ios::out | ios::trunc);

    myFile.write(reinterpret_cast<char *>(&n84), 4);   //  0
    myFile.write(string("CORD").c_str(), 4);              //  4
    //  8: Number of sets of coordinates, NAMD=0 ???
    myFile.write(reinterpret_cast<char *>(&numSets), 4);   
    // 12: Starting timestep of DCD file, should be never zero
    myFile.write(reinterpret_cast<char *>(&firstStep), 4);  
    // 16: Timesteps between DCD saves 
    myFile.write(reinterpret_cast<char *>(&numSteps), 4);  
    // 20: NAMD writes += numSteps 
    myFile.write(reinterpret_cast<char *>(&numSets), 4);   
    myFile.write(reinterpret_cast<char *>(&n0), 4);   // 24
    myFile.write(reinterpret_cast<char *>(&n0), 4);   // 28
    myFile.write(reinterpret_cast<char *>(&n0), 4);   // 32
    myFile.write(reinterpret_cast<char *>(&n0), 4);   // 36
    myFile.write(reinterpret_cast<char *>(&n0), 4);   // 40
    // 44 : length of a timestep
    myFile.write(reinterpret_cast<char *>(&timeStep), 4);   
    // 48 : unit cell, none=0, used=1
    myFile.write(reinterpret_cast<char *>(&n0), 4);   

    myFile.write(reinterpret_cast<char *>(&n0), 4);
    myFile.write(reinterpret_cast<char *>(&n0), 4);
    myFile.write(reinterpret_cast<char *>(&n0), 4);

    myFile.write(reinterpret_cast<char *>(&n0), 4);
    myFile.write(reinterpret_cast<char *>(&n0), 4);
    myFile.write(reinterpret_cast<char *>(&n0), 4);

    myFile.write(reinterpret_cast<char *>(&n0), 4);
    myFile.write(reinterpret_cast<char *>(&n0), 4);
    // Pretend to be Charmm 24
    myFile.write(reinterpret_cast<char *>(&n24), 4);   

    myFile.write(reinterpret_cast<char *>(&n84), 4);

    // Write DCD title record
    myFile.write(reinterpret_cast<char *>(&n164), 4);
    myFile.write(reinterpret_cast<char *>(&n2), 4);
    myFile.write(getRightFill(string("Remarks: File \'" +
                                     myFilename + "\' by " +
                                     getUserName() + ". ProtoMol ("
                                     + string(__DATE__) + " at "
                                     + string(__TIME__) + ")")
                              , 80).c_str(), 80);
    myFile.write(getRightFill(string("Remarks: " + myComment),
                              80).c_str(), 80);
    myFile.write(reinterpret_cast<char *>(&n164), 4);

    // Write DCD num-atoms record
    myFile.write(reinterpret_cast<char *>(&n4), 4);
    myFile.write(reinterpret_cast<char *>(&nAtoms), 4);
    myFile.write(reinterpret_cast<char *>(&n4), 4);
    if (myFile.fail()) {
      close();
      return false;
    }
    close();
  }

  myFile.clear();
  myFile.open(
    myFilename.c_str(), ios::binary | ios::out | ios::app);
  return !myFile.fail();
}

bool DCDTrajectoryWriter::write(const Vector3DBlock &coords) {
  const unsigned int count = coords.size();
  if (!reopen(count))
    return false;

  myX.resize(count);
  myY.resize(count);
  myZ.resize(count);

  for (unsigned int i = 0; i < count; ++i) {
    myX[i] = static_cast<float>(coords[i].x);
    myY[i] = static_cast<float>(coords[i].y);
    myZ[i] = static_cast<float>(coords[i].z);
    if (myIsLittleEndian != ISLITTLEENDIAN) {
      swapBytes(myX[i]);
      swapBytes(myY[i]);
      swapBytes(myZ[i]);
    }
  }

  int32 nAtoms = static_cast<int32>(count * 4);
  if (myIsLittleEndian != ISLITTLEENDIAN)
    swapBytes(nAtoms);
  myFile.write(reinterpret_cast<char *>(&nAtoms), sizeof(int32));
  myFile.write(reinterpret_cast<char *>(&(myX[0])), count * sizeof(float4));
  myFile.write(reinterpret_cast<char *>(&nAtoms), sizeof(int32));
  myFile.write(reinterpret_cast<char *>(&nAtoms), sizeof(int32));
  myFile.write(reinterpret_cast<char *>(&(myY[0])), count * sizeof(float4));
  myFile.write(reinterpret_cast<char *>(&nAtoms), sizeof(int32));
  myFile.write(reinterpret_cast<char *>(&nAtoms), sizeof(int32));
  myFile.write(reinterpret_cast<char *>(&(myZ[0])), count * sizeof(float4));
  myFile.write(reinterpret_cast<char *>(&nAtoms), sizeof(int32));

  close();
  return !myFile.fail();
}

void DCDTrajectoryWriter::setLittleEndian(bool littleEndian) {
  myIsLittleEndian = littleEndian;
}

void DCDTrajectoryWriter::setTimestep(Real timestep) {
  myTimeStep = timestep;
}

void DCDTrajectoryWriter::setFirststep(unsigned int firststep) {
  myFirstStep = firststep;
}

DCDTrajectoryWriter &operator<<(DCDTrajectoryWriter &dcdWriter,
                                const Vector3DBlock &coords) {
  dcdWriter.write(coords);
  return dcdWriter;
}

DCDTrajectoryWriter &operator<<(DCDTrajectoryWriter &dcdWriter,
                                const XYZ &xyz) {
  dcdWriter.write(xyz.coords);
  return dcdWriter;
}
