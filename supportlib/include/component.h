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

#ifndef COMPONENT_H
#define COMPONENT_H

/** \file component.h
 Contains the base Component class.
 */

#include <map>
#include <string>
#include <memory>
#include "slot.h"
#include "boost/shared_ptr.hpp"

// Define the CGKIT_SHARED variable
#ifdef DLL_EXPORT_COMPONENT
  #include "shared_export.h"
#else
  #include "shared.h"
#endif

namespace support3d {

using std::string;
using std::auto_ptr;


////////////////////////////////////////////////////////////////
// Slot descriptors
////////////////////////////////////////////////////////////////

struct SlotDescriptor
{
  virtual ~SlotDescriptor() {}
  virtual ISlot& getSlot() = 0;
};

struct DynamicSlotDescriptor : public SlotDescriptor
{
  auto_ptr<ISlot> slot;
  DynamicSlotDescriptor(auto_ptr<ISlot> aslot) : slot(aslot) {}
  ISlot& getSlot() { return *(slot.get()); }
};

struct StaticSlotDescriptor : public SlotDescriptor
{
  ISlot& slot;
  StaticSlotDescriptor(ISlot& aslot) : slot(aslot) {}
  ISlot& getSlot() { return slot; }
};


////////////////////////////////////////////////////////////////
// Component
////////////////////////////////////////////////////////////////

class CGKIT_SHARED SlotIterator
{
  private:
  std::map<string, SlotDescriptor*>::const_iterator it;
  std::pair<string, SlotDescriptor*> item;

  public:
  SlotIterator() { }
  SlotIterator(std::map<string, SlotDescriptor*>::const_iterator ait)
    : it(ait) 
  {}

  bool operator==(const SlotIterator& s) { return it==s.it; }
  bool operator!=(const SlotIterator& s) { return it!=s.it; }
  void operator++() { ++it; }
  void operator++(int) { it++; }
  std::pair<string, SlotDescriptor*>* operator->() 
  { 
    item.first=it->first; 
    item.second=it->second; 
    return &item; 
  }
};

/**
  This is the base class for all components.

  A component is a named container for slots.

  \see ISlot
 */
class CGKIT_SHARED Component
{
  public:
//  typedef std::map<string, SlotDescriptor*>::const_iterator SlotIterator;
  typedef support3d::SlotIterator SlotIterator;

  /// The name of the component.
  //Slot<string> name;
  string name;

  protected:

  /// Slot dictionary.
  std::map<string, SlotDescriptor*> slots;

  public:
  Component(string aname="");
  virtual ~Component();

  /**
    Return the component's name.

    \return Name
   */
  virtual string getName() const { return name; }

  /**
    Set a new name for the component.

  \param aname The new name to set.
   */
  virtual void setName(string aname) { name=aname; }

  bool hasSlot(const string& name) const;
  int numSlots() const;
  ISlot& slot(const string& name) const;
  
  void addSlot(const string& name, auto_ptr<ISlot> slot);
  void addSlot(const string& name, ISlot& slot);

  void removeSlot(const string& name);

//  SlotIterator slotsBegin() const { return slots.begin(); }
//  SlotIterator slotsEnd() const { return slots.end(); }
  SlotIterator slotsBegin() const { return SlotIterator(slots.begin()); }
  SlotIterator slotsEnd() const { return SlotIterator(slots.end()); }
};


}  // end of namespace

#endif
