/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Python Computer Graphics Kit.
 *
 * The Initial Developer of the Original Code is Matthias Baas.
 * Portions created by the Initial Developer are Copyright (C) 2004
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef COMMON_EXCEPTIONS_H
#define COMMON_EXCEPTIONS_H

#include <exception>
#include <string>

namespace support3d {

////////////////////////////////////////////////////////////////

/**
  Exception: RuntimeError.
 */
class ERuntimeError : public std::exception
{
  public:
  std::string msg;

  public:
  ERuntimeError(std::string amsg) : msg(amsg) {}
  ~ERuntimeError() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    return msg.c_str();
  }
};

////////////////////////////////////////////////////////////////

/**
  Exception: IOError.
 */
class EIOError : public std::exception
{
  public:
  std::string msg;

  public:
  EIOError(std::string amsg) : msg(amsg) {}
  ~EIOError() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    return msg.c_str();
  }
};

////////////////////////////////////////////////////////////////

/**
  Exception: Out of memory.
 */
class EMemoryError : public std::exception
{
  public:
  std::string msg;

  public:
  EMemoryError() {}
  EMemoryError(std::string amsg) : msg(amsg) {}
  ~EMemoryError() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    if (msg=="")
      return "Out of memory.";
    else
      return msg.c_str();
  }
};

////////////////////////////////////////////////////////////////

/**
  Exception: Index out of range.
 */
class EIndexError : public std::exception
{
  public:
  std::string msg;

  public:
  EIndexError() {}
  EIndexError(std::string amsg) : msg(amsg) {}
  ~EIndexError() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    if (msg=="")
      return "Index out of range.";
    else
      return msg.c_str();
  }
};

////////////////////////////////////////////////////////////////

/**
  Exception: Invalid value.
 */
class EValueError : public std::exception
{
  public:
  std::string msg;

  public:
  EValueError(std::string amsg) : msg(amsg) {}
  ~EValueError() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    return msg.c_str();
  }
};


////////////////////////////////////////////////////////////////

/**
  Exception: Divide by zero.
 */
class EZeroDivisionError : public std::exception
{
  public:
  std::string msg;

  public:
  EZeroDivisionError(std::string amsg="") : msg(amsg) 
  {
    if (msg=="")
    {
      msg = "Division by zero";
    }
  }
  ~EZeroDivisionError() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    return msg.c_str();
  }
};

////////////////////////////////////////////////////////////////

/**
  Exception: Key error.
 */
class EKeyError : public std::exception
{
  public:
  std::string msg;

  public:
  EKeyError(std::string amsg) : msg(amsg) {}
  ~EKeyError() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    return msg.c_str();
  }
};

////////////////////////////////////////////////////////////////

/**
  Exception: Not implemented
 */
class ENotImplementedError : public std::exception
{
  public:
  std::string msg;

  public:
  ENotImplementedError(std::string amsg) : msg(amsg) {}
  ~ENotImplementedError() throw() {}

  /// Return exception message.
  const char* what() const throw()
  {
    return msg.c_str();
  }
};


}  // end of namespace

#endif
