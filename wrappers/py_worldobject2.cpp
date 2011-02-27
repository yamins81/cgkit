/*
  WorldObject wrapper (part 2)
 */

#include "py_worldobject.h"


_WorldObjectChildIterator::_WorldObjectChildIterator(WorldObject& awo)
    : wo(awo)
{
  it = wo.childsBegin();
  itend = wo.childsEnd();
}

object _WorldObjectChildIterator::__iter__()
{
  return object(this);
}

boost::shared_ptr<WorldObject> _WorldObjectChildIterator::next()
{
  if (it==itend)
  {
    throw StopIteration();
  }
  else
  {
    boost::shared_ptr<WorldObject> res = it->second;
    it++;
    return res;
  }
}
  

//////////////////////////////////////////////////////////////////////

_WorldObjectChildIterator* iterChilds(WorldObject* self)
{
  return new _WorldObjectChildIterator(*self);
}


void class_WorldObject2(class_<WorldObject, bases<Component> >& WorldObject_class)
{
  void (WorldObject::*removeChild_instance)(boost::shared_ptr<WorldObject> child) = &WorldObject::removeChild;
  void (WorldObject::*removeChild_name)(string name) = &WorldObject::removeChild;

  // WorldObject
  WorldObject_class
    .def("getOffsetTransform", &WorldObject::getOffsetTransform,
	 return_value_policy<copy_const_reference>(),
	 "getOffsetTransform() -> mat4\n\n"
	 "Return the current offset transformation. This transformation is\n"
	 "given relative to the local object transformation.")

    .def("setOffsetTransform", &WorldObject::setOffsetTransform,
	 "setOffsetTransform(mat4)\n\n"
	 "Set the offset transformation. The transformation has to be given\n"
	 "relative to the local object transformation.")

    .def("getNumMaterials", &WorldObject::getNumMaterials,
	 "getNumMaterials() -> int\n\n"
	 "Return the current size of the material array.")

    .def("setNumMaterials", &WorldObject::setNumMaterials, arg("num"),
	 "setNumMaterials(num)\n\n"
	 "Set a new size for the material array.")

    .def("getMaterial", &WorldObject::getMaterial, (arg("idx")=0),
	 "getMaterial(idx=0) -> Material\n\n"
	 "Get a stored material. The method returns None if the given index\n"
	 "is out of range or there is no material stored at that position.")

    .def("setMaterial", &WorldObject::setMaterial, (arg("mat"), arg("idx")=0),
	 "setMaterial(mat, idx=0)\n\n"
	 "Set a new material. An IndexError exception is thrown if the index\n"
	 "is out of range.")

    .def("lenChilds", &WorldObject::lenChilds,
	 "lenChilds() -> int\n\n"
	 "Return the number of children objects.")

    .def("iterChilds", iterChilds, return_value_policy<manage_new_object>(),
	 "iterChilds() -> ChildIterator\n\n"
	 "Return an iterator that iterates over all children objects.")

    .def("hasChild", &WorldObject::hasChild,
    	 "hasChild(name) -> bool\n\n"
	 "Check if a children with a particular name does exist.")

    .def("child", &WorldObject::child,
	 "child(name) -> WorldObject\n\n"
	 "Return the children with a particluar name. A KeyError exception\n"
	 "is thrown if there is no children with the specified name.")
    
    .def("addChild", &WorldObject::addChild,
    	 "addChild(child)\n\n"
    	 "Add a new children world object to this object. A ValueError\n"
	 "exception is thrown if child was already added to another object.\n"
	 "In this case you have to remove the object from its previous parent\n"
	 "yourself. You also have to make sure that the name of child is\n"
	 "unique among the children of this object, otherwise a KeyError \n"
	 "exception is thrown.")

    .def("removeChild", removeChild_instance,
    	 "removeChild(child)\n\n"
    	 "Remove a children world object from this object. A KeyError\n"
	 "exception is thrown if child is not a children of this object.")

    .def("removeChild", removeChild_name,
	 "removeChild(name)\n\n"
	 "Remove a children world object from this object. A KeyError\n"
	 "exception is thrown if there is no children with the specified name.")

    .def("boundingBox", &WorldObject::boundingBox,
    	 "boundingBox() -> BoundingBox\n\n"
	 "Return the local axis aligned bounding box. The bounding box is\n"
	 "given with respect to the local transformation L (which is not \n"
	 "what you get from the transform slot of the world object).")

    .def("makeChildNameUnique", &WorldObject::makeChildNameUnique,
       	 "makeChildNameUnique(name) -> uniquename\n\n"
    	 "Modify a name so that it's unique among the children names. If\n"
	 "name is already the name of a children object, then it's modified\n"
	 "by adding/increasing a trailing number, otherwise it is returned\n"
	 "unchanged.")
  ;

}

