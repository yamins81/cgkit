/*
  WorldObject wrapper (part 3)
 */

#include "py_worldobject.h"

// Slot classes
void class_WorldObjectSlots()
{
  class_<TransformSlot, bases<Slot<mat4d> > >("TransformSlot", no_init);
  class_<PositionSlot, bases<Slot<vec3d> > >("PositionSlot", no_init);
  class_<RotationSlot, bases<Slot<mat3d> > >("RotationSlot", no_init);
  class_<ScaleSlot, bases<Slot<vec3d> > >("ScaleSlot", no_init);

  // Register the new ProceduralSlot types so that they can be used in Python
  class_<ProceduralSlot<double, WorldObject>, bases<Slot<double> > >("WorldObject_DoubleProceduralSlot", no_init);
  class_<ProceduralSlot<vec3d, WorldObject>, bases<Slot<vec3d> > >("WorldObject_Vec3ProceduralSlot", no_init);
  class_<ProceduralSlot<mat3d, WorldObject>, bases<Slot<mat3d> > >("WorldObject_Mat3ProceduralSlot", no_init);
  class_<ProceduralSlot<mat4d, WorldObject>, bases<Slot<mat4d> > >("WorldObject_Mat4ProceduralSlot", no_init);

  class_<_WorldObjectChildIterator>("_WorldObjectChildIterator", init<WorldObject&>())
    .def("__iter__", &_WorldObjectChildIterator::__iter__)
    .def("next", &_WorldObjectChildIterator::next)
  ;

}



