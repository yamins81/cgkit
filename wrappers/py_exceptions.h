/*
  C++-Versionen der Standard-Python Exceptions.
 */

#ifndef PYEXCEPTIONS_H
#define PYEXCEPTIONS_H

#include <boost/python.hpp>
#include <exception>

using namespace boost::python;
using namespace support3d;

/**
  StopIteration exception.
 */
class StopIteration : public std::exception
{
  public:
  StopIteration() { }
  ~StopIteration() throw() {}
};

/**
  Exception class.
 */
class EPyIndexError : public std::exception
{
  public:
  std::string msg;

  EPyIndexError(std::string amsg="") : msg(amsg) { }
  ~EPyIndexError() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    return msg.c_str();
  }
};


#endif
