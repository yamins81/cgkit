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

#define DLL_EXPORT_COMPONENT
#include "component.h"
#include "common_exceptions.h"

namespace support3d {

/**
  Constructor.

  \param aname The component's name
 */
Component::Component(string aname)
: name(aname), slots()
{
  DEBUGINFO1(this, "Component::Component(\"%s\")", aname.c_str());
  //addSlot("name", name);
}

/**
  Destructor.
 */
Component::~Component()
{
  DEBUGINFO(this, "Component::~Component()  (\""+name+"\")");
  // Remove all slots...
  while(slots.begin()!=slots.end())
  {
    removeSlot(slots.begin()->first);
  }
}

/**
  Check if a slot with a particular name does exist.

  \param name Slot name.
  \return True if a slot with the specified name exists, false otherwise.
 */
bool Component::hasSlot(const string& name) const
{
  return slots.find(name)!=slots.end();
}

/**
  Return the number of slots in this component.

  \return Number of slots.
 */
int Component::numSlots() const
{
  return slots.size();
}

/**
  Return the slot with the given name.

  A EKeyError exception is thrown if there's no slot with the 
  specified name.

  \param name The name of the slot that should be returned.
  \return Slot.
 */
ISlot& Component::slot(const string& name) const
{
  DEBUGINFO(this, "Component::slot(\""+name+"\")");
  
  SlotIterator it = slots.find(name);
  if (it==slots.end())
  {
    throw EKeyError("Slot \""+name+"\" does not exist.");
  }
  else
  {
    return it->second->getSlot();
  }
}
  
/**
  Add a dynamic slot to the component.

  Ownership of the slot is transferred to the component, i.e. when 
  the component is deleted the slot is deleted as well.
  A EKeyError exception is thrown if there is already a slot with
  the specified name.
  
  \param name Slot name
  \param slot Dynamically allocated slot
 */
void Component::addSlot(const string& name, auto_ptr<ISlot> slot)
{
  DEBUGINFO2(this, "Component::addSlot(\"%s\", auto_ptr(0x%x))", name.c_str(), (long)slot.get());
  if (hasSlot(name))
  {
    throw EKeyError("Slot \""+name+"\" already exists.");
  }
  DynamicSlotDescriptor* desc = new DynamicSlotDescriptor(slot);
  slots[name] = desc;
}

/**
  Add a static slot to the component.

  Ownership of the slot remains at the caller.
  A EKeyError exception is thrown if there is already a slot with
  the specified name.
  
  \warning As the ownership is \b not transferred you have to make sure 
           that the slot is \b not deleted before the component is deleted!
           The intention of this method is to enable components to have
           static slots that are defined right within their class which
           means the lifetimes of the slot and the component are automatically
           tied together.

  \param name Slot name
  \param slot Static slot
 */
void Component::addSlot(const string& name, ISlot& slot)
{
  DEBUGINFO2(this, "Component::addSlot(\"%s\", 0x%x)", name.c_str(), (long)&slot);
  if (hasSlot(name))
  {
    throw EKeyError("Slot \""+name+"\" already exists.");
  }
  StaticSlotDescriptor* desc = new StaticSlotDescriptor(slot);
  slots[name] = desc;
}

/** 
  Remove a slot.

  If the slot was added as a dynamic slot it is also deleted.
  A EKeyError exception is thrown if there's no slot with the 
  specified name.

  \param name The name of the slot that is to be removed.
 */
void Component::removeSlot(const string& name)
{
  DEBUGINFO1(this, "Component::removeSlot(\"%s\")", name.c_str());
  std::map<string, SlotDescriptor*>::iterator it = slots.find(name);
  if (it==slots.end())
  {
    throw EKeyError("Slot \""+name+"\" does not exist.");
  }
  else
  {
    // Delete the descriptor which will take care of deleting the slot (or not)
    delete it->second;
    // Remove the pointer to the deleted descriptor
    slots.erase(name);
  }
}


}  // end of namespace
