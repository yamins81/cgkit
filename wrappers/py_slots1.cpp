/*
 Slots.
 */

#include "py_slot.h"


long _slot_counter()
{
  return ISlot::_slot_counter;
}


void class_Slots()
{
  def("_slot_counter", _slot_counter);

  // Dependent
  class_<Dependent, DependentWrapper, boost::noncopyable>("Dependent")
    .def("onValueChanged", &DependentWrapper::base_onValueChanged)
    .def("onValueChanged", &DependentWrapper::base_onValueChanged_Array)
    .def("onResize", &DependentWrapper::base_onResize_Array)
    .def("queryResizeVeto", &DependentWrapper::base_queryResizeVeto)
  ;

  // ISlot
  class_<ISlot, bases<Dependent>, boost::noncopyable>("ISlot", no_init)
    .def("isSlotCompatible", &ISlot::isSlotCompatible)
    .def("typeName", &ISlot::typeName)
    .def("connect", &ISlot::connect)
    .def("disconnect", &ISlot::disconnect)
    .def("addDependent", &ISlot::addDependent)
    .def("removeDependent", &ISlot::removeDependent)
    .def("notifyDependents", &ISlot::notifyDependents)
    .def("getController", &ISlot::getController, return_internal_reference<>())
    .def("setController", &ISlot::setController)
  ;


  // Slots...
  SLOT("DoubleSlot",double);
  SLOT("IntSlot",int);
  SLOT("BoolSlot",bool);

}
