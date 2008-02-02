#include <protomol/io/Reader.h>

using namespace std;
using namespace ProtoMol;

//____ Reader
Reader::Reader() :
  File(ios::in) {}

Reader::Reader(const string &filename) :
  File(ios::in, filename) {
  File::open();
}

Reader::Reader(ios::openmode mode) :
  File(ios::in | mode) {}

Reader::Reader(ios::openmode mode,
               const string &filename) :
  File(ios::in | mode,
       filename) {
  File::open();
}

Reader::~Reader() {}

const string &Reader::getComment() const {
  return myComment;
}

