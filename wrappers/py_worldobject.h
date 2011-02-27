/*
  WorldObject wrapper header
 */

#include <boost/python.hpp>
#include "boost/shared_ptr.hpp"
#include "worldobject.h"
#include "py_exceptions.h"

using namespace boost::python;
using namespace support3d;

class _WorldObjectChildIterator
{
  private:
  WorldObject& wo;
  WorldObject::ChildIterator it;
  WorldObject::ChildIterator itend;

  public:
  _WorldObjectChildIterator(WorldObject& awo);

  object __iter__();
  boost::shared_ptr<WorldObject> next();
};

