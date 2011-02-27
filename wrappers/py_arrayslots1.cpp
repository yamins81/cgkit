/*
 ArraySlots.
 */

#include "py_slot.h"

// LinearSizeConstraint::getCoeffs() 
tuple getCoeffs(LinearSizeConstraint& c)
{
  int a, b;
  c.getCoeffs(a,b);
  return make_tuple(a,b);
}


void class_ArraySlots()
{
  // SizeConstraintBase
  class_<SizeConstraintBase, boost::noncopyable>("SizeConstraintBase", no_init)
    .def("getSize", &SizeConstraintBase::getSize)
  ;

  // UserSizeConstraint
  class_<UserSizeConstraint, bases<SizeConstraintBase> >("UserSizeConstraint", init<>())
      .def(init<int>())
      .def("setSize", &UserSizeConstraint::setSize)
    ;

  // LinearSizeConstraint
  class_<LinearSizeConstraint, bases<SizeConstraintBase, Dependent> >("LinearSizeConstraint", init<IArraySlot&, int, int>())
      .def("getCoeffs", getCoeffs)
      .def("setCoeffs", &LinearSizeConstraint::setCoeffs)
   ;

  // IArraySlot
  class_<IArraySlot, bases<ISlot>, boost::noncopyable>("IArraySlot", no_init)
    .def("size", &IArraySlot::size)
    .def("resize", &IArraySlot::resize)
    .def("isResizable", &IArraySlot::isResizable, (arg("size"), arg("ignorelocalconstraint")=false))
    .def("multiplicity", &IArraySlot::multiplicity)
    .def("copyValues", &IArraySlot::copyValues)
  ;

  ARRAYSLOT("DoubleArraySlot",double);
  ARRAYSLOT("IntArraySlot",int);
}
