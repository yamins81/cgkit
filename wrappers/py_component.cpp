/*
  Component wrapper
 */

#include <boost/python.hpp>
#include "component.h"
#include "py_exceptions.h"

using namespace boost::python;
using namespace support3d;

// Slot iterator
class _ComponentSlotIterator
{
  protected:
  Component& comp;
  Component::SlotIterator it;
  Component::SlotIterator itend;

  public:
  _ComponentSlotIterator(Component& acomp)
    : comp(acomp)
  {
    it = comp.slotsBegin();
    itend = comp.slotsEnd();
  }

  object __iter__()
  {
    return object(this);
  }

  string next()
  {
    if (it==itend)
    {
      throw StopIteration();
    }
    else
    {
      string res = it->first;
      it++;
      return res;
    }
  }
  
};

_ComponentSlotIterator* iterSlots(Component* self)
{
  return new _ComponentSlotIterator(*self);
}

// get/set for "name" property
/*string getName(Component* self)
{
  return self->name.getValue();
}

void setName(Component* self, string s)
{
  self->name.setValue(s);
}*/


void class_Component()
{
  //  void (Component::*addSlot)(const string&, auto_ptr<ISlot>) = &Component::addSlot;
  void (Component::*addSlot)(const string&, ISlot&) = &Component::addSlot;

  class_<Component>("Component", 
    "This is the base class for all scene components.\n\n"
    "A component is just something that has a name to identify it and that\n"
    "stores slot objects. Usually, the slots are used for providing inputs\n"
    "to the component which are used to generate outputs which can also be\n"
    "accessed via slots. The component is then the 'black box' that computes\n"
    "the output from the input.", 
		    init< optional<std::string> >())

    .add_property("name", &Component::getName, &Component::setName)
    //    .add_property("name", getName, setName)
    //    .def_readonly("name_slot", &Component::name)

    .def("hasSlot", &Component::hasSlot, arg("name"),
	 "hasSlot(name) -> bool\n\n"
	 "Check if a slot with a particular name does exist.")

    .def("numSlots", &Component::numSlots,
	 "numSlots() -> int\n\n"
	 "Return the number of slots in this component.")

    .def("slot", &Component::slot, arg("name"), return_internal_reference<>(),
	 "slot(name) -> Slot\n\n"
	 "Return the slot with the given name. A KeyError exception is thrown\n"
	 "if there's no slot with the specified name.")

    .def("addSlot", addSlot, (arg("name"), arg("slot")),
	 "addSlot(name, slot)\n\n"
	 "Add a new slot to the component. The first argument specifies the slot\n"
	 "name, the second argument must be a slot object. A KeyError exception\n"
	 "is thrown if there is already a slot with the specified name.")

    .def("removeSlot", &Component::removeSlot, arg("name"),
	 "removeSlot(name)\n\n"
	 "Remove an existing slot. A KeyError exception is thrown if there's\n"
	 "no slot with the specified name.")

    .def("iterSlots", iterSlots, return_value_policy<manage_new_object>(),
	 "iterSlots() -> SlotIterator\n\n"
	 "Return an iterator that iterates over all slot names.");
  ;

  class_<_ComponentSlotIterator>("_ComponentSlotIterator", init<Component&>())
    .def("__iter__", &_ComponentSlotIterator::__iter__)
    .def("next", &_ComponentSlotIterator::next)
  ;

}
