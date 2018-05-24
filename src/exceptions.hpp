#pragma once

#include <string>
#include <exception>

namespace NodeHDF5{
  class Exception : public std::exception {
  public:
    std::string message;
  public:
    Exception() : exception() {};
    Exception(std::string message) : exception() {
      this->message=message;
    };
    Exception(const Exception& orig) {
      this->message=orig.message;
    };
    virtual ~Exception() throw(){};
    virtual const char* what(){return message.c_str();};
  protected:

  };
}

