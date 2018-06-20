#include <v8.h>
#include <node.h>

#include "reference.hpp"

namespace NodeHDF5 {
  v8::Persistent<v8::Function> Reference::Constructor;
}

