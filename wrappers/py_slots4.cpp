/*
 Slots - part 4
 */

#include <iostream>
#include "py_slot.h"

// Specialized output operator of PySlots
// (as << is not defined on objects)
std::ostream& operator<<(std::ostream& os, const Slot<object>& slot)
{
  long addr = (long)&slot;
  os<<"Slot at 0x"<<std::setw(8)<<std::setfill('0')<<std::hex<<addr<<":";
  os<<" (object) flags:"<<slot.flags;
  if (slot.controller==0)
  {
    os<<"  no controller";
  }
  else
  {
    addr = (long)slot.controller;
    os<<"  controller:0x"<<std::setw(8)<<std::setfill('0')<<std::hex<<addr;
  }
  //  os<<std::endl<<"  "<<std::dec<<slot.dependents.size()<<" dependents:";
  //  for(int i=0; i<int(slot.dependents.size()); i++)
  //  {
  //    addr = (long)slot.dependents[i];
  //    os<<" 0x"<<std::setw(8)<<std::setfill('0')<<std::hex<<addr;
  //  }
  return os;
}

void class_Slots4()
{
  SLOT("QuatSlot",quatd);
  SLOT("PySlot",object);
}
